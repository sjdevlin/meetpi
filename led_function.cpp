#include <string.h>
#include <matrix_hal/everloop.h>
#include <matrix_hal/everloop_image.h>
#include <matrix_hal/matrixio_bus.h>

extern "C" int led_change(int input_from_c)
{
 
namespace hal = matrix_hal;

static hal::MatrixIOBus bus;
static hal::EverloopImage image1d(18);
static hal::Everloop everloop;

if (input_from_c == 0) 
   {

   if (!bus.Init()) return false;
   // this line just resizes the EverloopImage object to the number of LEDs on the board 
   everloop.Setup(&bus);

   // Clear all LEDs
/*   for (hal::LedValue &led : image1d.leds) 
      {
      led.red = 30;
      led.green = 20;
      led.blue = 0;
      led.white = 0;
      }
 */

   for (int i=0;i<18;i++)
      {
      image1d.leds[i].red = 10;
      image1d.leds[i].green = 0;
      image1d.leds[i].blue= 0;
      image1d.leds[i].white = 5;
      } 

  everloop.Write(&image1d);
   }
   else if (input_from_c == 1)
   {
//   bus.Init();
//   everloop.Setup(&bus);
   
   printf ("led call 2\n");
   for (int i=0;i<18;i++)
      {
      image1d.leds[i].red = 0;
      image1d.leds[i].green = 10;
      image1d.leds[i].blue= 10;
      image1d.leds[i].white = 0;
      } 
   everloop.Write(&image1d);
   }
return 1;
}
