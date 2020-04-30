/*
 * net_task.c
 *
 *  Created on: Feb 6, 2020
 *      Author: halitosisman
 *
 *
 *      Much of the initialization code here was copied from the Simplelink CC32xx SDK tcpecho example
 */


#include "net_task.h"


//*****************************************************************************
//                 GLOBAL VARIABLES
//*****************************************************************************

/* Connection state: (0) - connected, (negative) - disconnected              */
int32_t gApConnectionState = -1;
uint32_t gInitState = 0;
uint32_t memPtrCounterfree = 0;
bool gResetApplication = false;
static MQTTClient_Handle gMqttClient;
MQTTClient_Params MqttClientExmple_params;
unsigned short g_usTimerInts;

/* Receive task handle                                                       */
pthread_t g_rx_task_hndl = (pthread_t) NULL;
uint32_t gUiConnFlag = 0;

/* AP Security Parameters                                                    */
SlWlanSecParams_t SecurityParams = { 0 };

/* Client ID                                                                 */
/* If ClientId isn't set, the MAC address of the device will be copied into  */
/* the ClientID parameter.                                                   */
char ClientId[13] = {'\0'};

/* Client User Name and Password                                             */
const char *ClientUsername = "qskyndde";
const char *ClientPassword = "S2FnBWAyw4xP";

/* Subscription topics and qos values                                        */
char *topic[SUBSCRIPTION_TOPIC_COUNT] =
{ SUBSCRIPTION_TOPIC0, SUBSCRIPTION_TOPIC1 };

unsigned char qos[SUBSCRIPTION_TOPIC_COUNT] =
{ MQTT_QOS_2, MQTT_QOS_2 };

/* Message Queue                                                             */
mqd_t g_PBQueue;
pthread_t mqttThread = (pthread_t) NULL;
pthread_t appThread = (pthread_t) NULL;
timer_t g_timer;


/* Printing new line                                                         */
char lineBreak[] = "\n\r";

//*****************************************************************************
//                 Banner VARIABLES
//*****************************************************************************
#ifdef  SECURE_CLIENT

char *Mqtt_Client_secure_files[CLIENT_NUM_SECURE_FILES] = {"ca-cert.pem"};

/*Initialization structure to be used with sl_ExtMqtt_Init API. In order to  */
/*use secured socket method, the flag MQTTCLIENT_NETCONN_SEC, cipher,        */
/*n_files and secure_files must be configured.                               */
/*certificates also must be programmed  ("ca-cert.pem").                     */
/*The first parameter is a bit mask which configures server address type and */
/*security mode.                                                             */
/*Server address type: IPv4, IPv6 and URL must be declared with The          */
/*corresponding flag.                                                        */
/*Security mode: The flag MQTTCLIENT_NETCONN_SEC enables the security (TLS)  */
/*which includes domain name verification and certificate catalog            */
/*verification, those verifications can be disabled by adding to the bit mask*/
/*MQTTCLIENT_NETCONN_SKIP_DOMAIN_NAME_VERIFICATION and                       */
/*MQTTCLIENT_NETCONN_SKIP_CERTIFICATE_CATALOG_VERIFICATION flags             */
/*Example: MQTTCLIENT_NETCONN_IP6 | MQTTCLIENT_NETCONN_SEC |                 */
/*MQTTCLIENT_NETCONN_SKIP_CERTIFICATE_CATALOG_VERIFICATION                   */
/*For this bit mask, the IPv6 address type will be in use, the security      */
/*feature will be enable and the certificate catalog verification will be    */
/*skipped.                                                                   */
/*Note: The domain name verification requires URL Server address type        */
/*      otherwise, this verification will be disabled.                       */
MQTTClient_ConnParams Mqtt_ClientCtx =
{
    MQTTCLIENT_NETCONN_IP4 | MQTTCLIENT_NETCONN_SEC,
    SERVER_IP_ADDRESS,  //SERVER_ADDRESS,
    SECURED_PORT_NUMBER, //  PORT_NUMBER
    SLNETSOCK_SEC_METHOD_SSLv3_TLSV1_2,
    SLNETSOCK_SEC_CIPHER_FULL_LIST,
    CLIENT_NUM_SECURE_FILES,
    Mqtt_Client_secure_files
};

#else
MQTTClient_ConnParams Mqtt_ClientCtx =
{
    MQTTCLIENT_NETCONN_URL,
    SERVER_ADDRESS,
    PORT_NUMBER, 0, 0, 0,
    NULL
};
#endif

/* Initialize the will_param structure to the default will parameters        */
MQTTClient_Will will_param =
{
    WILL_TOPIC,
    WILL_MSG,
    WILL_QOS,
    WILL_RETAIN
};

//*****************************************************************************
//
//! MQTT_SendMsgToQueue - Utility function that receive msgQueue parameter and
//! tries to push it the queue with minimal time for timeout of 0.
//! If the queue isn't full the parameter will be stored and the function
//! will return 0.
//! If the queue is full and the timeout expired (because the timeout parameter
//! is 0 it will expire immediately), the parameter is thrown away and the
//! function will return -1 as an error for full queue.
//!
//! \param[in] struct msgQueue *queueElement
//!
//! \return 0 on success, -1 on error
//
//*****************************************************************************
int32_t MQTT_SendMsgToQueue(struct msgQueue *queueElement)
{
    struct timespec abstime = {0};

    clock_gettime(CLOCK_REALTIME, &abstime);

    if(g_PBQueue)
    {
        /* send message to the queue                                        */
        if(mq_timedsend(g_PBQueue, (char *) queueElement,
                        sizeof(struct msgQueue), 0, &abstime) == 0)
        {
            return(0);
        }
    }
    return(-1);
}


//*****************************************************************************
//
//! Application startup display on UART
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
static void DisplayBanner(char * AppName)
{
    UART_PRINT("\n\n\n\r");
    UART_PRINT("\t\t *************************************************\n\r");
    UART_PRINT("\t\t    CC32xx %s Application       \n\r", AppName);
    UART_PRINT("\t\t *************************************************\n\r");
    UART_PRINT("\n\n\n\r");
}

void * MqttClientThread(void * pvParameters)
{
    struct msgQueue queueElement;
    struct msgQueue queueElemRecv;

    MQTTClient_run((MQTTClient_Handle)pvParameters);

    queueElement.event = LOCAL_CLIENT_DISCONNECTION;
    queueElement.msgPtr = NULL;

    /*write message indicating disconnect Broker message.                   */
    if(MQTT_SendMsgToQueue(&queueElement))
    {
        UART_PRINT(
            "\n\n\rQueue is full, throw first msg and send the new one\n\n\r");
        mq_receive(g_PBQueue, (char*) &queueElemRecv, sizeof(struct msgQueue),
                   NULL);
        MQTT_SendMsgToQueue(&queueElement);
    }

    pthread_exit(0);

    return(NULL);
}

//*****************************************************************************
//
//! Task implementing MQTT Server plus client bridge
//!
//! This function
//!    1. Initializes network driver and connects to the default AP
//!    2. Initializes the mqtt client ans server libraries and set up MQTT
//!       with the remote broker.
//!    3. set up the button events and their callbacks(for publishing)
//!    4. handles the callback signals
//!
//! \param  none
//!
//! \return None
//!
//*****************************************************************************

void publish_command(void * device, struct Command* command)
{
    if (device == NULL)
    {
        if ((command->command[0] == 'u') || (command->command[0] == 'k'))
        {
            // system wide update or  kill
            clean_device_list(&(device_list));
        }

        if (command->command[0] == 'u')
        {
            // System
            MQTTClient_publish(gMqttClient,
                               (char *) SYSTEM_PUB_TOPIC,
                               SYSTEM_PUB_TOPIC_LEN,
                               (char *) command->command,
                               command->command_len,
                               MQTT_QOS_2 | ((RETAIN_ENABLE) ? MQTT_PUBLISH_RETAIN : 0));
            MQTTClient_publish(gMqttClient,
                               (char *) SYSTEM_PUB_TOPIC,
                               SYSTEM_PUB_TOPIC_LEN,
                               (char *) CLEAR_COMMAND,
                               CLEAR_COMMAND_LEN,
                               MQTT_QOS_2 | ((RETAIN_ENABLE) ? MQTT_PUBLISH_RETAIN : 0));
        }
        else
        {
            // Syst
            MQTTClient_publish(gMqttClient,
                               (char *) SYST_PUB_TOPIC,
                               SYST_PUB_TOPIC_LEN,
                               (char *) command->command,
                               command->command_len,
                               MQTT_QOS_2 | ((RETAIN_ENABLE) ? MQTT_PUBLISH_RETAIN : 0));
            MQTTClient_publish(gMqttClient,
                               (char *) SYST_PUB_TOPIC,
                               SYST_PUB_TOPIC_LEN,
                               (char *) CLEAR_COMMAND,
                               CLEAR_COMMAND_LEN,
                               MQTT_QOS_2 | ((RETAIN_ENABLE) ? MQTT_PUBLISH_RETAIN : 0));
        }
    }
    else
    {
        // Device
        MQTTClient_publish(gMqttClient,
                           (char *)(((struct Light*) device)->pub_topic),
                           23,
                           (char *) command->command,
                           command->command_len,
                           MQTT_QOS_2 | ((RETAIN_ENABLE) ? MQTT_PUBLISH_RETAIN : 0));
    }
}


void add_command(struct Command* head, char * name, int name_len, char * ID, char * command_name, int command_len)
{
    struct Command* command = (struct Command*) malloc(sizeof(struct Command));

    strcpy(command->name, name);
    command->name_len = name_len;
    strcpy(command->ID, ID);
    strcpy(command->command, command_name);
    command->command_len = command_len;

    while (head->next != NULL)
    {
        head = head->next;
    }

    head->next = command;
    command->prev = head;
    command->next = NULL;
}

void init_command_list()
{
    //Light
    ////////// Basic
    struct Command* L = (struct Command*) malloc(sizeof(struct Command));
    strcpy(L->name, "Get Status");
    L->name_len = 11;
    strcpy(L->ID, "L__");
    strcpy(L->command, LIGHT_GET_STATUS);
    L->command_len = LIGHT_GET_STATUS_LEN;
    L->prev = NULL;
    L->next = NULL;
    command_list.L = L;

    add_command(L, "Set On", 7, "L__", LIGHT_SET_ON, LIGHT_SET_ON_LEN);
    add_command(L, "Set Off", 8, "L__", LIGHT_SET_OFF, LIGHT_SET_OFF_LEN);
    add_command(L, "Reset Status", 13, "L__", LIGHT_RESET_STATUS, LIGHT_RESET_STATUS_LEN);
    add_command(L, "Kill Device", 12, "___", KILL_DEVICE, KILL_DEVICE_LEN);

    ////////// Dimmable
    struct Command* LD = (struct Command*) malloc(sizeof(struct Command));
    strcpy(LD->name, "Get Status");
    LD->name_len = 11;
    strcpy(LD->ID, "LD_");
    strcpy(LD->command, LIGHT_GET_STATUS);
    LD->command_len = LIGHT_GET_STATUS_LEN;
    LD->prev = NULL;
    LD->next = NULL;
    command_list.LD = LD;

    add_command(LD, "Set On", 7, "L__", LIGHT_SET_ON, LIGHT_SET_ON_LEN);
    add_command(LD, "Set Off", 8, "L__", LIGHT_SET_OFF, LIGHT_SET_OFF_LEN);
    add_command(LD, "Reset Status", 13, "L__", LIGHT_RESET_STATUS, LIGHT_RESET_STATUS_LEN);
    add_command(LD, "Save Status", 12, "LD_", LIGHT_SAVE_STATUS, LIGHT_SAVE_STATUS_LEN);
    add_command(LD, "Pause", 6, "LD_", LIGHT_PAUSE, LIGHT_PAUSE_LEN);
    add_command(LD, "Resume", 7, "LD_", LIGHT_RESUME, LIGHT_RESUME_LEN);
    add_command(LD, "Dim", 4, "LD_", LIGHT_DIM, LIGHT_DIM_LEN);
    add_command(LD, "Brighten", 9, "LD_", LIGHT_BRIGHTEN, LIGHT_BRIGHTEN_LEN);
    add_command(LD, "Set Brighten", 13, "LD_", LIGHT_SET_BRIGHTNESS, LIGHT_SET_BRIGHTNESS_LEN);
    add_command(LD, "Kill Device", 12, "___", KILL_DEVICE, KILL_DEVICE_LEN);
    //////////


    //Motor
    ////////// - Basic
    struct Command* M = (struct Command*) malloc(sizeof(struct Command));
    strcpy(M->name, "Get Status");
    M->name_len = 11;
    strcpy(M->ID, "M__");
    strcpy(M->command, MOTOR_GET_STATUS);
    M->command_len = MOTOR_GET_STATUS_LEN;
    M->prev = NULL;
    M->next = NULL;
    command_list.M = M;

    add_command(M, "Set Forward", 12, "M__", MOTOR_SET_FORWARD, MOTOR_SET_FORWARD_LEN);
    add_command(M, "Set Off", 8, "M__", MOTOR_SET_OFF, MOTOR_SET_OFF_LEN);
    add_command(M, "Reset Status", 13, "M__", MOTOR_RESET_STATUS, MOTOR_RESET_STATUS_LEN);
    add_command(M, "Kill Device", 12, "___", KILL_DEVICE, KILL_DEVICE_LEN);

    ////////// - Reversable
    struct Command* MR = (struct Command*) malloc(sizeof(struct Command));
    strcpy(MR->name, "Get Status");
    MR->name_len = 11;
    strcpy(MR->ID, "M__");
    strcpy(MR->command, MOTOR_GET_STATUS);
    MR->command_len = MOTOR_GET_STATUS_LEN;
    MR->prev = NULL;
    MR->next = NULL;
    command_list.MR = MR;

    add_command(MR, "Set Forward", 12, "M__", MOTOR_SET_FORWARD, MOTOR_SET_FORWARD_LEN);
    add_command(MR, "Set Off", 8, "M__", MOTOR_SET_OFF, MOTOR_SET_OFF_LEN);
    add_command(MR, "Set Backwards", 14, "MR_", MOTOR_SET_BACKWARDS, MOTOR_SET_BACKWARDS_LEN);
    add_command(MR, "Reset Status", 13, "M__", MOTOR_RESET_STATUS, MOTOR_RESET_STATUS_LEN);
    add_command(MR, "Kill Device", 12, "___", KILL_DEVICE, KILL_DEVICE_LEN);

    ////////// - Setable
    struct Command* MS = (struct Command*) malloc(sizeof(struct Command));
    strcpy(MS->name, "Get Status");
    MS->name_len = 11;
    strcpy(MS->ID, "M__");
    strcpy(MS->command, MOTOR_GET_STATUS);
    MS->command_len = MOTOR_GET_STATUS_LEN;
    MS->prev = NULL;
    MS->next = NULL;
    command_list.MS = MS;

    add_command(MS, "Set Forward", 12, "M__", MOTOR_SET_FORWARD, MOTOR_SET_FORWARD_LEN);
    add_command(MS, "Set Off", 8, "M__", MOTOR_SET_OFF, MOTOR_SET_OFF_LEN);
    add_command(MS, "Reset Status", 13, "M__", MOTOR_RESET_STATUS, MOTOR_RESET_STATUS_LEN);
    add_command(MS, "Save Status", 12, "M_S", MOTOR_SAVE_STATUS, MOTOR_SAVE_STATUS_LEN);
    add_command(MS, "Pause", 6, "M_S", MOTOR_PAUSE, MOTOR_PAUSE_LEN);
    add_command(MS, "Resume", 7, "M_S", MOTOR_RESUME, MOTOR_RESUME_LEN);
    add_command(MS, "Slow Down", 10, "M_S", MOTOR_SLOW, MOTOR_SLOW_LEN);
    add_command(MS, "Speed Up", 9, "M_S", MOTOR_SPEED, MOTOR_SPEED_LEN);
    add_command(MS, "Set Speed", 10, "M_S", MOTOR_SET_SPEED, MOTOR_SET_SPEED_LEN);
    add_command(MS, "Kill Device", 12, "___", KILL_DEVICE, KILL_DEVICE_LEN);

    ////////// - Reversable Setable
    struct Command* MRS = (struct Command*) malloc(sizeof(struct Command));
    strcpy(MRS->name, "Get Status");
    MRS->name_len = 11;
    strcpy(MRS->ID, "M__");
    strcpy(MRS->command, MOTOR_GET_STATUS);
    MRS->command_len = MOTOR_GET_STATUS_LEN;
    MRS->prev = NULL;
    MRS->next = NULL;
    command_list.MRS = MRS;

    add_command(MRS, "Set Forward", 12, "M__", MOTOR_SET_FORWARD, MOTOR_SET_FORWARD_LEN);
    add_command(MRS, "Set Off", 8, "M__", MOTOR_SET_OFF, MOTOR_SET_OFF_LEN);
    add_command(MRS, "Set Backwards", 14, "MR_", MOTOR_SET_BACKWARDS, MOTOR_SET_BACKWARDS_LEN);
    add_command(MRS, "Reset Status", 13, "M__", MOTOR_RESET_STATUS, MOTOR_RESET_STATUS_LEN);
    add_command(MRS, "Save Status", 12, "M_S", MOTOR_SAVE_STATUS, MOTOR_SAVE_STATUS_LEN);
    add_command(MRS, "Pause", 6, "M_S", MOTOR_PAUSE, MOTOR_PAUSE_LEN);
    add_command(MRS, "Resume", 7, "M_S", MOTOR_RESUME, MOTOR_RESUME_LEN);
    add_command(MRS, "Slow Down", 10, "M_S", MOTOR_SLOW, MOTOR_SLOW_LEN);
    add_command(MRS, "Speed Up", 9, "M_S", MOTOR_SPEED, MOTOR_SPEED_LEN);
    add_command(MRS, "Set Speed", 10, "M_S", MOTOR_SET_SPEED, MOTOR_SET_SPEED_LEN);
    add_command(MRS, "Kill Device", 12, "L__", KILL_DEVICE, KILL_DEVICE_LEN);
    //////////

    //Accel
    ////////// - Basic
    struct Command* A = (struct Command*) malloc(sizeof(struct Command));
    strcpy(A->name, "Get Status");
    A->name_len = 11;
    strcpy(A->ID, "A__");
    strcpy(A->command, ACCEL_GET_STATUS);
    A->command_len = ACCEL_GET_STATUS_LEN;
    A->prev = NULL;
    A->next = NULL;
    command_list.A = A;

    add_command(A, "Set On", 7, "A__", ACCEL_SET_ON, ACCEL_SET_ON_LEN);
    add_command(A, "Set Off", 8, "A__", ACCEL_SET_OFF, ACCEL_SET_OFF_LEN);
    add_command(A, "Kill Device", 12, "___", KILL_DEVICE, KILL_DEVICE_LEN);

    ////////// - Setable
    struct Command* AF = (struct Command*) malloc(sizeof(struct Command));
    strcpy(AF->name, "Get Status");
    AF->name_len = 11;
    strcpy(AF->ID, "AF_");
    strcpy(AF->command, ACCEL_GET_STATUS);
    AF->command_len = ACCEL_GET_STATUS_LEN;
    AF->prev = NULL;
    AF->next = NULL;
    command_list.AF = AF;

    add_command(AF, "Set On", 7, "A__", ACCEL_SET_ON, ACCEL_SET_ON_LEN);
    add_command(AF, "Set Off", 8, "A__", ACCEL_SET_OFF, ACCEL_SET_OFF_LEN);
    add_command(AF, "Set Freq", 9, "AF_", ACCEL_SET_FREQ, ACCEL_SET_FREQ_LEN);
    add_command(AF, "Kill Device", 12, "___", KILL_DEVICE, KILL_DEVICE_LEN);
    //////////

    //Temp
    ////////// - Basic
    struct Command* T = (struct Command*) malloc(sizeof(struct Command));
    strcpy(T->name, "Get Status");
    T->name_len = 11;
    strcpy(T->ID, "T__");
    strcpy(T->command, TEMP_GET_STATUS);
    T->command_len = TEMP_GET_STATUS_LEN;
    T->prev = NULL;
    T->next = NULL;
    command_list.T = T;

    add_command(T, "Set On", 7, "T__", TEMP_SET_ON, TEMP_SET_ON_LEN);
    add_command(T, "Set Off", 8, "T__", TEMP_SET_OFF, TEMP_SET_OFF_LEN);
    add_command(T, "Kill Device", 12, "___", KILL_DEVICE, KILL_DEVICE_LEN);


    ////////// - Setable
    struct Command* TF = (struct Command*) malloc(sizeof(struct Command));
    strcpy(TF->name, "Get Status");
    TF->name_len = 11;
    strcpy(TF->ID, "TF_");
    strcpy(TF->command, TEMP_GET_STATUS);
    TF->command_len = TEMP_GET_STATUS_LEN;
    TF->prev = NULL;
    TF->next = NULL;
    command_list.TF = TF;

    add_command(TF, "Set On", 7, "T__", TEMP_SET_ON, TEMP_SET_ON_LEN);
    add_command(TF, "Set Off", 8, "T__", TEMP_SET_OFF, TEMP_SET_OFF_LEN);
    add_command(TF, "Set Freq", 9, "TF_", TEMP_SET_FREQ, TEMP_SET_FREQ_LEN);
    add_command(TF, "Kill Device", 12, "___", KILL_DEVICE, KILL_DEVICE_LEN);
    //////////

    //System
    //////////
    struct Command* S = (struct Command*) malloc(sizeof(struct Command));
    strcpy(S->name, "Get Status");
    S->name_len = 11;
    strcpy(S->ID, "S__");
    strcpy(S->command, SYSTEM_GET_STATUS);
    S->command_len = SYSTEM_GET_STATUS_LEN;
    S->prev = NULL;
    S->next = NULL;
    command_list.S = S;

    add_command(S, "Reset All Devices", 18, "S__", SYSTEM_RESET_STATUS, SYSTEM_RESET_STATUS_LEN);
    add_command(S, "Update All Devices", 19, "S__", SYSTEM_UPDATE, SYSTEM_UPDATE_LEN);
    add_command(S, "Kill Device", 12, "___", KILL_DEVICE, KILL_DEVICE_LEN);
    //////////
}

int valid_command(char *ID, struct Command command)
{
    // check type; Light, Motor, Accel, Temp
    if (ID[0] == command.ID[0])
    {
        if ((ID[1] == command.ID[1]) || (command.ID[1] == '_'))
        {
            if ((ID[2] == command.ID[1]) || (command.ID[2] == '_'))
            {
                //Valid command
                return 1;
            }
        }
    }

    return 0;
}


void add_device(int type, void * new_device)
{
    //Light=0,Motor=1,Accel=2,Temp=3
    struct Light* device_group;
    struct Light* prev_device;
    struct Light* new_device_cast = (struct Light*) new_device;
    switch(type)
    {
        case 0:
            device_group = (struct Light*) (device_list.Light);
            break;
        case 1:
            device_group = (struct Light*) (device_list.Motor);
            break;
        case 2:
            device_group = (struct Light*) (device_list.Accel);
            break;
        case 3:
            device_group = (struct Light*) (device_list.Temp);
            break;
    }

    if (device_group == NULL)
    {
        //adds first device if its first of the type
        switch(type)
        {
            case 0:
                device_list.Light = (struct Light*) new_device;
                device_list.Light_len = 1;
                break;
            case 1:
                device_list.Motor = (struct Motor *) new_device;
                device_list.Motor_len = 1;
                break;
            case 2:
                device_list.Accel = (struct Accel*) new_device;
                device_list.Accel_len = 1;
                break;
            case 3:
                device_list.Temp = (struct Temp*) new_device;
                device_list.Temp_len = 1;
                break;
        }
    }
    else
    {
        while (device_group != NULL)
        {
            if (strcmp(device_group->name, new_device_cast->name) == 0)
            {
                UART_PRINT("Duplicate Device Registration\n\r");
                break;
            }


            switch(type)
            {
                case 0:
                    device_list.Light_len += 1;
                    break;
                case 1:
                    device_list.Motor_len += 1;
                    break;
                case 2:
                    device_list.Accel_len += 1;
                    break;
                case 3:
                    device_list.Temp_len += 1;
                    break;
            }

            if (strcmp(device_group->name, new_device_cast->name) < 0)
            {
                prev_device = device_group;
                device_group = device_group->prev;

                if (device_group == NULL)
                {
                    //add device to end of list
                    new_device_cast->next = prev_device;
                    prev_device->prev = new_device_cast;
                    break;
                }
                else if (strcmp(device_group->name, new_device_cast->name) > 0)
                {
                    //insert device between list
                    new_device_cast->next = prev_device;
                    new_device_cast->prev = device_group;

                    prev_device->prev = new_device_cast;
                    device_group->next = new_device_cast;
                    break;
                }
            }
            else
            {
                prev_device = device_group;
                device_group = device_group->next;

                if (device_group == NULL)
                {
                    //add device to end of list
                    new_device_cast->prev = prev_device;
                    prev_device->next = new_device_cast;
                    break;
                }
                else if (strcmp(device_group->name, new_device_cast->name) < 0)
                {
                    //insert device between list
                    new_device_cast->prev = prev_device;
                    new_device_cast->next = device_group;

                    prev_device->next = new_device_cast;
                    device_group->prev = new_device_cast;
                    break;
                }
            }
        }
    }
}

// Update device status
void find_device(int type, char * id, float status)
{
    //Light=0,Motor=1,Accel=2,Temp=3
    struct Light* device_group;

    switch(type)
    {
        case 0:
            device_group = (struct Light*) device_list.Light;
            break;
        case 1:
            device_group = (struct Light*) device_list.Motor;
            break;
        case 2:
            device_group = (struct Light*) device_list.Accel;
            break;
        case 3:
            device_group = (struct Light*) device_list.Temp;
            break;
    }

    if (device_group != NULL)
    {
        while (device_group->prev != NULL)
        {
            device_group = device_group->prev;
        }
    }

    while (device_group != NULL)
    {
        if (strcmp(device_group->ID, id) == 0)
        {
            device_group->status = status;
            break;
        }

        device_group = device_group->next;
    }
}

// Free one device type
void clean_device_type(void *device)
{
    if (device != NULL)
    {
        while (((struct Light*)device)->prev != NULL)
        {
            device = (void *) ((struct Light*)device)->prev;
        }

        while (device != NULL)
        {
            void * prev_device = device;
            device = (void *) ((struct Light*)device)->next;
            free(prev_device);
        }
    }
}

// Free all current stored devices
void clean_device_list(struct Device_List* devices)
{
    clean_device_type(devices->Light);
    devices->Light = NULL;
    clean_device_type(devices->Motor);
    devices->Motor = NULL;
    clean_device_type(devices->Accel);
    devices->Accel = NULL;
    clean_device_type(devices->Temp);
    devices->Temp = NULL;
}





void * MqttClient(void *pvParameters)
{
    struct msgQueue queueElemRecv;
    long lRetVal = -1;
    char *tmpBuff;

    // Init Command Lists
    init_command_list();

    /*Initializing Client and Subscribing to the Broker.                     */
    if(gApConnectionState >= 0)
    {
        lRetVal = MqttClient_start();
        if(lRetVal == -1)
        {
            UART_PRINT("MQTT Client lib initialization failed\n\r");
            pthread_exit(0);
            return(NULL);
        }
    }

    /*handling the signals from various callbacks including the push button  */
    /*prompting the client to publish a msg on PUB_TOPIC OR msg received by  */
    /*the server on enrolled topic(for which the on-board client ha enrolled)*/
    /*from a local client(will be published to the remote broker by the      */
    /*client) OR msg received by the client from the remote broker (need to  */
    /*be sent to the server to see if any local client has subscribed on the */
    /*same topic).                                                           */
    for(;; )
    {
        /*waiting for signals                                                */
        mq_receive(g_PBQueue, (char*) &queueElemRecv, sizeof(struct msgQueue),
                   NULL);


        switch(queueElemRecv.event)
        {
        /*msg received by client from remote broker (on a topic      */
        /*subscribed by local client)                                */
        case MSG_RECV_BY_CLIENT:

            tmpBuff = (char *) ((char *) queueElemRecv.msgPtr + 12);
            // status/#
            if(strncmp(tmpBuff, SUBSCRIPTION_TOPIC0, 7) == 0)
            {
                // light, motor, accel, tempr, or special
                char *tmpType = (char *)((char *)tmpBuff + 7);

                if (strncmp(tmpType, "SPECIAL/", 7) != 0)
                {
                    UART_PRINT("\n\rTopic:%s\n\rMsg:%s\n\r\r\n",tmpBuff, tmpBuff + queueElemRecv.topLen + 1);
                    // compnent id
                    char *tmpID = (char *)((char *)tmpType + 6);

                    char * tmpmsg = (char *)(tmpBuff + queueElemRecv.topLen + 1);
                    char * end;

                    float status = strtof(tmpmsg, &end);

                    if (strncmp(tmpType, "light/", 6) == 0)
                    {
                        find_device(0, tmpID, status);
                    }
                    else if (strncmp(tmpType, "motor/", 6) == 0)
                    {
                        find_device(1, tmpID, status);
                    }
                    else if (strncmp(tmpType, "accel/", 6) == 0)
                    {
                        find_device(2, tmpID, status);
                    }
                    else if (strncmp(tmpType, "tempr/", 6) == 0)
                    {
                        find_device(3, tmpID, status);
                    }
                }
                else
                {
                    //special case handler
                    char * tmpCode = (char *)(tmpBuff + queueElemRecv.topLen + 1);
                    char * end;

                    int code = strtol(tmpCode, &end, 10);
                    UART_PRINT("Special Command: %d\n\r\r\n",code);

                    switch(code)
                    {
                    case 0:
                        //Update connected devices
                      //  update_devices();
                        break;
                    }

                }
            }
            // new/#
            else if(strncmp(tmpBuff, SUBSCRIPTION_TOPIC1,
                            4) == 0)
            {
                //UART_PRINT("\n\rTopic:%s\n\rMsg:%s\n\r\r",tmpBuff, tmpBuff + queueElemRecv.topLen + 1);

                // light, motor, accel, tempr
                char *tmpType = (char *)((char *)tmpBuff + 4);
                // compnent id
                char *tmpID = (char *)((char *)tmpType + 6);

                // name
                char * tmpmsg = (char *)(tmpBuff + queueElemRecv.topLen + 1);
                char * end;
                int tmpname_length = strtol(tmpmsg, &end, 10);
                char *tmpname = (char *)((char *)tmpmsg + 5);

                if (strncmp(tmpType, "light/", 6) == 0)
                {
                    struct Light* tmp_Light = (struct Light*) malloc(sizeof(struct Light));
                    tmp_Light->next = NULL;
                    tmp_Light->prev = NULL;

                    // Update device name
                    strcpy(tmp_Light->name, tmpname);
                    tmp_Light->name_len = tmpname_length;

                    // Update device ID (8 units)
                    strcpy(tmp_Light->ID, (char *)tmpID);

                    // Update device publish topic
                    char tmpTopic[23];
                    strcpy(tmpTopic, (char *)LIGHT_COMMAND_PUB_TOPIC);
                    strcat((char *) tmpTopic, (char *)tmpID);
                    strcpy(tmp_Light->pub_topic, (char *) tmpTopic);

                    // Update device Type
                    char *dimmable;
                    if (tmpID[1] == 'D')
                    {
                        tmp_Light->dimmable = true;
                        dimmable = "True";
                    }
                    else
                    {
                        tmp_Light->dimmable = false;
                        dimmable = "False";
                    }

                    // Update device Status to default value
                    tmp_Light->status = 0;

                    UART_PRINT("Light Device Connected\n\r"
                            "Device Name: %s\n\rDevice ID: %s\n\r"
                            "Light Dimmable: %s\n\rPub Topic: %s\n\r\n\r\r\n"
                            ,tmp_Light->name, tmp_Light->ID,
                            dimmable, tmp_Light->pub_topic);

                    MQTTClient_publish(gMqttClient, (char *) tmp_Light->pub_topic,
                                       23,
                                       (char *) LIGHT_CONNECTION_CONFIRM,
                                       LIGHT_CONNECTION_CONFIRM_LEN,
                                       MQTT_QOS_2 | ((RETAIN_ENABLE) ? MQTT_PUBLISH_RETAIN : 0));

                    add_device(0, tmp_Light);
                }
                else if (strncmp(tmpType, "motor/", 6) == 0)
                {
                    struct Motor* tmpMotor = (struct Motor*) malloc(sizeof(struct Motor));
                    tmpMotor->next = NULL;
                    tmpMotor->prev = NULL;

                    // Update device name
                    strcpy(tmpMotor->name, tmpname);
                    tmpMotor->name_len = tmpname_length;

                    // Update device ID (8 units)
                    strcpy(tmpMotor->ID, tmpID);

                    // Update device publish topic
                    char tmpTopic[23];
                    strcpy(tmpTopic, (char *)MOTOR_COMMAND_PUB_TOPIC);
                    strcat(tmpTopic, (char *)tmpID);
                    strcpy(tmpMotor->pub_topic, tmpTopic);

                    // Update device Type
                    char *reversable;
                    if (tmpID[1] == 'R')
                    {
                        // Reversable
                        tmpMotor->reversable = true;
                        reversable = "True";
                    }
                    else
                    {
                        tmpMotor->reversable = true;
                        reversable = "False";
                    }
                    char *setable;
                    if (tmpID[2] == 'S')
                    {
                        // Setable
                        tmpMotor->setable = true;
                        setable = "True";
                    }
                    else
                    {
                        tmpMotor->setable = true;
                        setable = "False";
                    }

                    // Update device Status to default value
                    tmpMotor->status = 0;

                    UART_PRINT("Motor Device Connected\n\r"
                            "Device Name: %s\n\rDevice ID: %s\n\r"
                            "Motor Reversable: %s\n\r"
                            "Motor Setable: %s\n\r"
                            "Pub Topic: %s\n\r\n\r\r\n"
                            ,tmpMotor->name, tmpMotor->ID,
                            reversable, setable, tmpMotor->pub_topic);

                    MQTTClient_publish(gMqttClient, (char *) tmpMotor->pub_topic,
                                       23,
                                       (char *) MOTOR_CONNECTION_CONFIRM,
                                       MOTOR_CONNECTION_CONFIRM_LEN,
                                       MQTT_QOS_2 | ((RETAIN_ENABLE) ? MQTT_PUBLISH_RETAIN : 0));

                    add_device(1, tmpMotor);
                }
                else if (strncmp(tmpType, "accel/", 6) == 0)
                {
                    struct Accel* tmp_Accel = (struct Accel*) malloc(sizeof(struct Accel));
                    tmp_Accel->next = NULL;
                    tmp_Accel->prev = NULL;

                    // Update device name
                    strcpy(tmp_Accel->name, tmpname);
                    tmp_Accel->name_len = tmpname_length;

                    // Update device ID (8 units)
                    strcpy(tmp_Accel->ID, tmpID);

                    // Update device publish topic
                    char tmpTopic[23];
                    strcpy(tmpTopic, (char *)ACCEL_COMMAND_PUB_TOPIC);
                    strcat(tmpTopic, (char *)tmpID);
                    strcpy(tmp_Accel->pub_topic, tmpTopic);

                    // Update device Type
                    char *setable;
                    if (tmpID[1] == 'F')
                    {
                        //Setable Freqency
                        tmp_Accel->setable = true;
                        setable = "True";
                    }
                    else
                    {
                        tmp_Accel->setable = false;
                        setable = "False";
                    }

                    // Update device Status to default value
                    tmp_Accel->status = 0;

                    UART_PRINT("Accel Device Connected\n\r"
                            "Device Name: %s\n\rDevice ID: %s\n\r"
                            "Accel Setable: %s\n\rPub Topic: %s\n\r\n\r\r\n"
                            ,tmp_Accel->name, tmp_Accel->ID,
                            setable, tmp_Accel->pub_topic);

                    MQTTClient_publish(gMqttClient, (char *) tmp_Accel->pub_topic,
                                       23,
                                       (char *) ACCEL_CONNECTION_CONFIRM,
                                       ACCEL_CONNECTION_CONFIRM_LEN,
                                       MQTT_QOS_2 | ((RETAIN_ENABLE) ? MQTT_PUBLISH_RETAIN : 0));

                    add_device(2, tmp_Accel);
                }
                else if (strncmp(tmpType, "tempr/", 6) == 0)
                {
                    struct Temp* tmp_Temp = (struct Temp*) malloc(sizeof(struct Temp));
                    tmp_Temp->next = NULL;
                    tmp_Temp->prev = NULL;

                    // Update device name
                    strcpy(tmp_Temp->name, tmpname);
                    tmp_Temp->name_len = tmpname_length;

                    // Update device ID (8 units)
                    strcpy(tmp_Temp->ID, tmpID);

                    // Update device publish topic
                    char tmpTopic[23];
                    strcpy(tmpTopic, (char *)TEMP_COMMAND_PUB_TOPIC);
                    strcat(tmpTopic, (char *)tmpID);
                    strcpy(tmp_Temp->pub_topic, tmpTopic);

                    // Update device Type
                    char *setable;
                    if (tmpID[1] == 'F')
                    {
                        //Setable Freqency
                        tmp_Temp->setable = true;
                        setable = "True";
                    }
                    else
                    {
                        tmp_Temp->setable = false;
                        setable = "False";
                    }

                    // Update device Status to default value
                    tmp_Temp->status = 0;

                    UART_PRINT("Temp Device Connected\n\r"
                            "Device Name: %s\n\rDevice ID: %s\n\r"
                            "Temp Setable: %s\n\rPub Topic: %s\n\r\n\r\r\n"
                            ,tmp_Temp->name, tmp_Temp->ID,
                            setable, tmp_Temp->pub_topic);

                    MQTTClient_publish(gMqttClient, (char *) tmp_Temp->pub_topic,
                                       23,
                                       (char *) TEMP_CONNECTION_CONFIRM,
                                       TEMP_CONNECTION_CONFIRM_LEN,
                                       MQTT_QOS_2 | ((RETAIN_ENABLE) ? MQTT_PUBLISH_RETAIN : 0));

                    add_device(3, tmp_Temp);
                }
            }

            free(queueElemRecv.msgPtr);
            break;

            /*On-board client disconnected from remote broker, only      */
            /*local MQTT network will work                               */
        case LOCAL_CLIENT_DISCONNECTION:
            UART_PRINT("\n\rOn-board Client Disconnected\n\r\r\n");
            gUiConnFlag = 0;
            break;

            /*Push button for full restart check                         */
        case DISC_PUSH_BUTTON_PRESSED:
            gResetApplication = true;
            break;

        case THREAD_TERMINATE_REQ:
            gUiConnFlag = 0;
            pthread_exit(0);
            return(NULL);

        case USR_CMD:
            publish_command(outgoing_state.selected_device, outgoing_state.selected_command);
            break;

        default:
            sleep(1);
            break;
        }
    }
}

//*****************************************************************************
//
//! This function connect the MQTT device to an AP with the SSID which was
//! configured in SSID_NAME definition which can be found in Network_if.h file,
//! if the device can't connect to to this AP a request from the user for other
//! SSID will appear.
//!
//! \param  none
//!
//! \return None
//!
//*****************************************************************************
int32_t Mqtt_IF_Connect()
{
    int32_t lRetVal;
    char SSID_Remote_Name[32];
    int8_t Str_Length;

    memset(SSID_Remote_Name, '\0', sizeof(SSID_Remote_Name));
    Str_Length = strlen(SSID_NAME);

    if(Str_Length)
    {
        /*Copy the Default SSID to the local variable                        */
        strncpy(SSID_Remote_Name, SSID_NAME, Str_Length);
    }

    /*Display Application Banner                                             */
    DisplayBanner(APPLICATION_NAME);

    /*Reset The state of the machine                                         */
    Network_IF_ResetMCUStateMachine();

    /*Start the driver                                                       */
    lRetVal = Network_IF_InitDriver(ROLE_STA);
    if(lRetVal < 0)
    {
        UART_PRINT("Failed to start SimpleLink Device\n\r", lRetVal);
        return(-1);
    }

    /*Initialize AP security params                                          */
    SecurityParams.Key = (signed char *) SECURITY_KEY;
    SecurityParams.KeyLen = strlen(SECURITY_KEY);
    SecurityParams.Type = SECURITY_TYPE;

    /*Connect to the Access Point                                            */
    lRetVal = Network_IF_ConnectAP(SSID_Remote_Name, SecurityParams);
    if(lRetVal < 0)
    {
        UART_PRINT("Connection to an AP failed\n\r");
        return(-1);
    }

    return(0);
}

//*****************************************************************************
//!
//! MQTT Start - Initialize and create all the items required to run the MQTT
//! protocol
//!
//! \param  none
//!
//! \return None
//!
//*****************************************************************************
void Mqtt_start()
{
    int32_t threadArg = 100;
    pthread_attr_t pAttrs;
    struct sched_param priParam;
    int32_t retc = 0;
    mq_attr attr;
    unsigned mode = 0;

    /*sync object for inter thread communication                             */
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct msgQueue);
    g_PBQueue = mq_open("g_PBQueue", O_CREAT, mode, &attr);

    if(g_PBQueue == NULL)
    {
        UART_PRINT("MQTT Message Queue create fail\n\r");
        gInitState &= ~MQTT_INIT_STATE;
        return;
    }

    /*Set priority and stack size attributes                                 */
    pthread_attr_init(&pAttrs);
    priParam.sched_priority = NET_TASK_PRIORITY; // 2
    retc = pthread_attr_setschedparam(&pAttrs, &priParam);
    retc |= pthread_attr_setstacksize(&pAttrs, MQTTTHREADSIZE);
    retc |= pthread_attr_setdetachstate(&pAttrs, PTHREAD_CREATE_DETACHED);

    if(retc != 0)
    {
        gInitState &= ~MQTT_INIT_STATE;
        UART_PRINT("MQTT thread create fail\n\r");
        return;
    }

    retc = pthread_create(&mqttThread, &pAttrs, MqttClient, (void *) &threadArg);
    if(retc != 0)
    {
        gInitState &= ~MQTT_INIT_STATE;
        UART_PRINT("MQTT thread create fail\n\r");
        return;
    }

    gInitState &= ~MQTT_INIT_STATE;
}

//*****************************************************************************
//!
//! MQTT Stop - Close the client instance and free all the items required to
//! run the MQTT protocol
//!
//! \param  none
//!
//! \return None
//!
//*****************************************************************************

void Mqtt_Stop()
{
    struct msgQueue queueElement;
    struct msgQueue queueElemRecv;

    if(gApConnectionState >= 0)
    {
        Mqtt_ClientStop(1);
    }

    queueElement.event = THREAD_TERMINATE_REQ;
    queueElement.msgPtr = NULL;

    /*write message indicating publish message                               */
    if(MQTT_SendMsgToQueue(&queueElement))
    {
        UART_PRINT(
            "\n\n\rQueue is full, throw first msg and send the new one\n\n\r");
        mq_receive(g_PBQueue, (char*) &queueElemRecv, sizeof(struct msgQueue),
                   NULL);
        MQTT_SendMsgToQueue(&queueElement);
    }

    sleep(2);

    mq_close(g_PBQueue);
    g_PBQueue = NULL;

    sl_Stop(SL_STOP_TIMEOUT);
    UART_PRINT("\n\r Client Stop completed\r\n");
}

int32_t MqttClient_start()
{
    int32_t lRetVal = -1;
    int32_t iCount = 0;

    int32_t threadArg = 100;
    pthread_attr_t pAttrs;
    struct sched_param priParam;

    MqttClientExmple_params.clientId = ClientId;
    MqttClientExmple_params.connParams = &Mqtt_ClientCtx;
    MqttClientExmple_params.mqttMode31 = MQTT_3_1;
    MqttClientExmple_params.blockingSend = true;

    gInitState |= CLIENT_INIT_STATE;

    /*Initialize MQTT client lib                                             */
    gMqttClient = MQTTClient_create(MqttClientCallback,
                                    &MqttClientExmple_params);
    if(gMqttClient == NULL)
    {
        /*lib initialization failed                                          */
        gInitState &= ~CLIENT_INIT_STATE;
        return(-1);
    }

    /*Open Client Receive Thread start the receive task. Set priority and    */
    /*stack size attributes                                                  */
    pthread_attr_init(&pAttrs);
    priParam.sched_priority = NET_TASK_MQTT_CLIENT_PRIORITY; // 2
    lRetVal = pthread_attr_setschedparam(&pAttrs, &priParam);
    lRetVal |= pthread_attr_setstacksize(&pAttrs, RXTASKSIZE);
    lRetVal |= pthread_attr_setdetachstate(&pAttrs, PTHREAD_CREATE_DETACHED);
    lRetVal |=
        pthread_create(&g_rx_task_hndl, &pAttrs, MqttClientThread,
                       (void *) &threadArg);
    if(lRetVal != 0)
    {
        UART_PRINT("Client Thread Create Failed failed\n\r");
        gInitState &= ~CLIENT_INIT_STATE;
        return(-1);
    }


    /*setting will parameters                                                */
    MQTTClient_set(gMqttClient, MQTTClient_WILL_PARAM, &will_param,
                   sizeof(will_param));

#ifdef CLNT_USR_PWD
    /*Set user name for client connection                                    */
    MQTTClient_set(gMqttClient, MQTTClient_USER_NAME, (void *)ClientUsername,
                   strlen(
                       (char*)ClientUsername));

    /*Set password                                                           */
    MQTTClient_set(gMqttClient, MQTTClient_PASSWORD, (void *)ClientPassword,
                   strlen(
                       (char*)ClientPassword));
#endif
    /*Initiate MQTT Connect                                                  */
    if(gApConnectionState >= 0)
    {
#if CLEAN_SESSION == false
        bool clean = CLEAN_SESSION;
        MQTTClient_set(gMqttClient, MQTTClient_CLEAN_CONNECT, (void *)&clean,
                       sizeof(bool));
#endif
        /*The return code of MQTTClient_connect is the ConnACK value that
           returns from the server */
        lRetVal = MQTTClient_connect(gMqttClient);

        /*negative lRetVal means error,
           0 means connection successful without session stored by the server,
           greater than 0 means successful connection with session stored by
           the server */
        if(0 > lRetVal)
        {
            /*lib initialization failed                                      */
            UART_PRINT("Connection to broker failed, Error code: %d\n\r",
                       lRetVal);

            gUiConnFlag = 0;
        }
        else
        {
            gUiConnFlag = 1;
        }
        /*Subscribe to topics when session is not stored by the server       */
        if((gUiConnFlag == 1) && (0 == lRetVal))
        {
            uint8_t subIndex;
            MQTTClient_SubscribeParams subscriptionInfo[
                SUBSCRIPTION_TOPIC_COUNT];

            for(subIndex = 0; subIndex < SUBSCRIPTION_TOPIC_COUNT; subIndex++)
            {
                subscriptionInfo[subIndex].topic = topic[subIndex];
                subscriptionInfo[subIndex].qos = qos[subIndex];
            }

            if(MQTTClient_subscribe(gMqttClient, subscriptionInfo,
                                    SUBSCRIPTION_TOPIC_COUNT) < 0)
            {
                UART_PRINT("\n\r Subscription Error \n\r");
                MQTTClient_disconnect(gMqttClient);
                gUiConnFlag = 0;
            }
            else
            {
                for(iCount = 0; iCount < SUBSCRIPTION_TOPIC_COUNT; iCount++)
                {
                    UART_PRINT("Client subscribed on %s\n\r,", topic[iCount]);
                }
            }
        }
    }

    gInitState &= ~CLIENT_INIT_STATE;

    return(0);
}

//*****************************************************************************
//!
//! MQTT Client stop - Unsubscribe from the subscription topics and exit the
//! MQTT client lib.
//!
//! \param  none
//!
//! \return None
//!
//*****************************************************************************

void Mqtt_ClientStop(uint8_t disconnect)
{
    uint32_t iCount;

    MQTTClient_UnsubscribeParams subscriptionInfo[SUBSCRIPTION_TOPIC_COUNT];

    for(iCount = 0; iCount < SUBSCRIPTION_TOPIC_COUNT; iCount++)
    {
        subscriptionInfo[iCount].topic = topic[iCount];
    }

    MQTTClient_unsubscribe(gMqttClient, subscriptionInfo,
                           SUBSCRIPTION_TOPIC_COUNT);
    for(iCount = 0; iCount < SUBSCRIPTION_TOPIC_COUNT; iCount++)
    {
        UART_PRINT("Unsubscribed from the topic %s\r\n", topic[iCount]);
    }
    gUiConnFlag = 0;

    /*exiting the Client library                                             */
    MQTTClient_delete(gMqttClient);
}

//*****************************************************************************
//!
//! Utility function which prints the borders
//!
//! \param[in] ch  -  hold the charater for the border.
//! \param[in] n   -  hold the size of the border.
//!
//! \return none.
//!
//*****************************************************************************

void printBorder(char ch,
                 int n)
{
    int i = 0;

    for(i = 0; i < n; i++)
    {
        putch(ch);
    }
}

//*****************************************************************************
//!
//! Set the ClientId with its own mac address
//! This routine converts the mac address which is given
//! by an integer type variable in hexadecimal base to ASCII
//! representation, and copies it into the ClientId parameter.
//!
//! \param  macAddress  -   Points to string Hex.
//!
//! \return void.
//!
//*****************************************************************************
int32_t SetClientIdNamefromMacAddress()
{
    int32_t ret = 0;
    uint8_t Client_Mac_Name[2];
    uint8_t Index;
    uint16_t macAddressLen = SL_MAC_ADDR_LEN;
    uint8_t macAddress[SL_MAC_ADDR_LEN];

    /*Get the device Mac address */
    ret = sl_NetCfgGet(SL_NETCFG_MAC_ADDRESS_GET, 0, &macAddressLen,
                       &macAddress[0]);

    /*When ClientID isn't set, use the mac address as ClientID               */
    if(ClientId[0] == '\0')
    {
        /*6 bytes is the length of the mac address                           */
        for(Index = 0; Index < SL_MAC_ADDR_LEN; Index++)
        {
            /*Each mac address byte contains two hexadecimal characters      */
            /*Copy the 4 MSB - the most significant character                */
            Client_Mac_Name[0] = (macAddress[Index] >> 4) & 0xf;
            /*Copy the 4 LSB - the least significant character               */
            Client_Mac_Name[1] = macAddress[Index] & 0xf;

            if(Client_Mac_Name[0] > 9)
            {
                /*Converts and copies from number that is greater than 9 in  */
                /*hexadecimal representation (a to f) into ascii character   */
                ClientId[2 * Index] = Client_Mac_Name[0] + 'a' - 10;
            }
            else
            {
                /*Converts and copies from number 0 - 9 in hexadecimal       */
                /*representation into ascii character                        */
                ClientId[2 * Index] = Client_Mac_Name[0] + '0';
            }
            if(Client_Mac_Name[1] > 9)
            {
                /*Converts and copies from number that is greater than 9 in  */
                /*hexadecimal representation (a to f) into ascii character   */
                ClientId[2 * Index + 1] = Client_Mac_Name[1] + 'a' - 10;
            }
            else
            {
                /*Converts and copies from number 0 - 9 in hexadecimal       */
                /*representation into ascii character                        */
                ClientId[2 * Index + 1] = Client_Mac_Name[1] + '0';
            }
        }
    }
    return(ret);
}

//*****************************************************************************
//!
//! Utility function which Display the app banner
//!
//! \param[in] appName     -  holds the application name.
//! \param[in] appVersion  -  holds the application version.
//!
//! \return none.
//!
//*****************************************************************************

int32_t DisplayAppBanner(char* appName,
                         char* appVersion)
{
    int32_t ret = 0;
    uint8_t macAddress[SL_MAC_ADDR_LEN];
    uint16_t macAddressLen = SL_MAC_ADDR_LEN;
    uint16_t ConfigSize = 0;
    uint8_t ConfigOpt = SL_DEVICE_GENERAL_VERSION;
    SlDeviceVersion_t ver = {0};

    ConfigSize = sizeof(SlDeviceVersion_t);

    /*Print device version info. */
    ret =
        sl_DeviceGet(SL_DEVICE_GENERAL, &ConfigOpt, &ConfigSize,
                     (uint8_t*)(&ver));

    /*Print device Mac address */
    ret |= (int32_t)sl_NetCfgGet(SL_NETCFG_MAC_ADDRESS_GET, 0, &macAddressLen,
                       &macAddress[0]);

    UART_PRINT(lineBreak);
    UART_PRINT("\t");
    printBorder('=', 44);
    UART_PRINT(lineBreak);
    UART_PRINT("\t   %s Example Ver: %s",appName, appVersion);
    UART_PRINT(lineBreak);
    UART_PRINT("\t");
    printBorder('=', 44);
    UART_PRINT(lineBreak);
    UART_PRINT(lineBreak);
    UART_PRINT("\t CHIP: 0x%x",ver.ChipId);
    UART_PRINT(lineBreak);
    UART_PRINT("\t MAC:  %d.%d.%d.%d",ver.FwVersion[0],ver.FwVersion[1],
               ver.FwVersion[2],
               ver.FwVersion[3]);
    UART_PRINT(lineBreak);
    UART_PRINT("\t PHY:  %d.%d.%d.%d",ver.PhyVersion[0],ver.PhyVersion[1],
               ver.PhyVersion[2],
               ver.PhyVersion[3]);
    UART_PRINT(lineBreak);
    UART_PRINT("\t NWP:  %d.%d.%d.%d",ver.NwpVersion[0],ver.NwpVersion[1],
               ver.NwpVersion[2],
               ver.NwpVersion[3]);
    UART_PRINT(lineBreak);
    UART_PRINT("\t ROM:  %d",ver.RomVersion);
    UART_PRINT(lineBreak);
    UART_PRINT("\t HOST: %s", SL_DRIVER_VERSION);
    UART_PRINT(lineBreak);
    UART_PRINT("\t MAC address: %02x:%02x:%02x:%02x:%02x:%02x", macAddress[0],
               macAddress[1], macAddress[2], macAddress[3], macAddress[4],
               macAddress[5]);
    UART_PRINT(lineBreak);
    UART_PRINT(lineBreak);
    UART_PRINT("\t");
    printBorder('=', 44);
    UART_PRINT(lineBreak);
    UART_PRINT(lineBreak);

    return(ret);
}
struct mq_attr i2cbox =
{
 .mq_maxmsg = 1,
 .mq_msgsize = sizeof(char)
};
struct mq_attr netbox =
{
 .mq_maxmsg = 1,
 .mq_msgsize = sizeof(struct msgQueue)
};
struct mq_attr guibox =
{
 .mq_maxmsg = 1,
 .mq_msgsize = sizeof(char)
};

void net_task(void * par) {

    ((FGthread_arg_t *) par)->mailroom[I2C_THREAD_ID] = mq_open("i2cbox", O_CREAT, 0, &i2cbox);
    ((FGthread_arg_t *) par)->mailroom[GUI_THREAD_ID] = mq_open("guibox", O_CREAT, 0, &guibox);
    ((FGthread_arg_t *) par)->mailroom[NET_THREAD_ID] = mq_open("netbox", O_CREAT, 0, &netbox);

    pthread_mutexattr_t attr;

    pthread_mutexattr_init(&attr);
    pthread_mutex_init(&list_sync, &attr);

    uint32_t count = 0;

    pthread_attr_t pAttrs_spawn;
    pthread_t spawn_thread = (pthread_t) NULL;
    struct sched_param priParam;
    int32_t retc = 0;
    /*Initialize SlNetSock layer with CC31xx/CC32xx interface */
    SlNetIf_init(0);
    SlNetIf_add(SLNETIF_ID_1, "CC32xx",
                (const SlNetIf_Config_t *)&SlNetIfConfigWifi,
                SLNET_IF_WIFI_PRIO);

    SlNetSock_init(0);
    SlNetUtil_init(0);

    /*Create the sl_Task                                                     */
    pthread_attr_init(&pAttrs_spawn);
    priParam.sched_priority = SPAWN_TASK_PRIORITY;
    retc = pthread_attr_setschedparam(&pAttrs_spawn, &priParam);
    retc |= pthread_attr_setstacksize(&pAttrs_spawn, TASKSTACKSIZE * 2);
    retc |= pthread_attr_setdetachstate
            (&pAttrs_spawn, PTHREAD_CREATE_DETACHED);

    retc = pthread_create(&spawn_thread, &pAttrs_spawn, sl_Task, NULL);

    if(retc != 0)
    {
        UART_PRINT("could not create simplelink task\n\r");
        while(1)
        {
            ;
        }
    }

    // https://e2e.ti.com/support/wireless-connectivity/wifi/f/968/t/707300 interesting
    retc = sl_Start(0, 0, 0);
    if(retc < 0)
    {
        /*Handle Error */
        UART_PRINT("\n sl_Start failed\n");
        while(1)
        {
            ;
        }
    }

    /*Output device information to the UART terminal */
    retc = DisplayAppBanner(APPLICATION_NAME, APPLICATION_VERSION);
    /*Set the ClientId with its own mac address */
    retc |= SetClientIdNamefromMacAddress();


    retc = sl_Stop(SL_STOP_TIMEOUT);
    if(retc < 0)
    {
        /*Handle Error */
        UART_PRINT("\n sl_Stop failed\n");
        while(1)
        {
            ;
        }
    }

    if(retc < 0)
    {
        /*Handle Error */
//       UART_PRINT("mqtt_client - Unable to retrieve device information \n");
        while(1)
        {
            ;
        }
    }

    while(1)
    {
        gResetApplication = false;
        topic[0] = SUBSCRIPTION_TOPIC0;
        topic[1] = SUBSCRIPTION_TOPIC1;
        gInitState = 0;

        /*Connect to AP                                                      */
        gApConnectionState = Mqtt_IF_Connect();

        gInitState |= MQTT_INIT_STATE;
        /*Run MQTT Main Thread (it will open the Client and Server)          */
        Mqtt_start();

        /*Wait for init to be completed!!!                                   */
        while(gInitState != 0)
        {
            UART_PRINT(".");
            sleep(1);
        }
        UART_PRINT(".\r\n");

        while(gResetApplication == false)
        {
            sleep(10);
        }

        UART_PRINT("TO Complete - Closing all threads and resources\r\n");

        /*Stop the MQTT Process                                              */
        Mqtt_Stop();

        UART_PRINT("reopen MQTT # %d  \r\n", ++count);
    }
}
