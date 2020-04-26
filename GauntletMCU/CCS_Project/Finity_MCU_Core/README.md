This is the development project for the Finity Gauntlet. It was based off TI's empty legacy project in the Simplelink SDK.

4/26/20 Lessons Learned
TI driver bugs discovered so far:
-grlib functions use a globally visible Graphics_Context instead of the one passed
in that they should be using.
-http://e2e.ti.com/support/wireless-connectivity/wifi/f/968/t/706641 this one is still here, and the solution 
mentioned still works. To add to this post - One needs to update the imports.mak file in ti/(simplelink dir) with 
the proper paths, comment out the compiler types that aren't being used in this file, and then call the make function
from the XDC tools directory from TI's drivers directory. This allows one to recompile TI's libararies. 

-One shouldn't expect reference local variables created in main from FreeRTOS threads. Moving a FreeRTOS variable to 
the global section made pointers to it not cause faults. 