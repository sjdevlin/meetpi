#include <matrix_hal/everloop.h>
#include <matrix_hal/everloop_image.h>
#include <matrix_hal/matrixio_bus.h>

namespace hal = matrix_hal;


  // Everloop Initialization
  hal::MatrixIOBus bus;
  if (!bus.Init()) return false;
  hal::EverloopImage image1d(bus.MatrixLeds());
  hal::Everloop everloop;
  everloop.Setup(&bus);

  // Clear all LEDs
  for (hal::LedValue &led : image1d.leds) {
    led.red = 0;
    led.green = 0;
    led.blue = 0;
    led.white = 0;
  }
  everloop.Write(&image1d);

    for (int i = 0; i < num_participants; i++) {
      // Convert from angle to pots index
      int brightness = (participant[i].talking) ? MAX_BRIGHTNESS : 0;
      image1d.leds[participant[i].led_num].red = brightness * colour[i].red;
      image1d.leds[participant[i].led_num].green = brightness * colour[i].green;
      image1d.leds[participant[i].led_num].blue = brightness * colour[i].blue;
      image1d.leds[participant[i].led_num].white = 0;
      

      //printf ("led %d index %d person %d brightness %d \n", i,index_pots, position_person_number[index_pots],brightness);
    }
 
    everloop.Write(&image1d);
 
}
}
