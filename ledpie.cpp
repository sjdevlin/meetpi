#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <matrix_hal/everloop.h>
#include <matrix_hal/everloop_image.h>
#include <matrix_hal/matrixio_bus.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <array>
#include <iostream>
#include <json-c/json.h>

namespace hal = matrix_hal;

//pots = potential sources

// LOCATIONS_COUNT : Number of locations where sound can come from.
#define LOCATIONS_COUNT 54
// MAX_VALUE : max value of energy
#define MAX_VALUE 200
// INCREMENT : multipler to amplify change in odas E value
#define INCREMENT 40
// DECREMENT : controls delay in the dimming
#define DECREMENT 1
// MIN_THRESHOLD: Filters out low energy targets from odas
#define MIN_THRESHOLD 20
// MAX_BRIGHTNESS: 0 - 255
#define MAX_BRIGHTNESS 12
// MAX_PARTICIPANTS: Max number of people in meeting
#define MAX_PARTICIPANTS 9


//these variables hold the value from odas of x,y and energy_array
double x, y, z, E;
int tracked_source_id;

int position_person_number[LOCATIONS_COUNT];
int total_talk_time[MAX_PARTICIPANTS];
int num_participants = 0;
int energy_array[LOCATIONS_COUNT];

struct rgb_value {int red; int green; int blue;} ;
struct rgb_value participant_colour[MAX_PARTICIPANTS] = {
  {12,0,0},
  {0,12,0},
  {0,0,12},
  {6,6,0},
  {6,0,6},
  {0,6,6},
  {4,4,2},
  {4,2,4},
  {2,4,4} };


const double led_angles_in_matrixvoice[18] = {170, 150, 130, 110, 90,  70,
                                      50,  30,  10,  350, 330, 310,
                                      290, 270, 250, 230, 210, 190};

void capture_energy_level_at_location() {
  // Convert x,y to angle. TODO: See why x axis from ODAS is inverted
  double angle_xy = fmodf((atan2(y, x) * (180.0 / M_PI)) + 360, 360);
  // Convert angle to index
  int i_angle = angle_xy / 360 * LOCATIONS_COUNT;  // convert degrees to index
  // Set energy for this angle
  energy_array[i_angle] += INCREMENT * E ;
  // Set limit at MAX_VALUE
  energy_array[i_angle] =
      energy_array[i_angle] > MAX_VALUE ? MAX_VALUE : energy_array[i_angle];
}

void fade_energy_levels() {
  for (int i = 0; i < LOCATIONS_COUNT; i++) {
    energy_array[i] -= (energy_array[i] > 0) ? DECREMENT : 0;
  }

}

void json_parse_array(json_object *jobj, char *key) {
  // Forward Declaration
  void json_parse(json_object * jobj);
  enum json_type type;
  json_object *jarray = jobj;
  if (key) {
    if (json_object_object_get_ex(jobj, key, &jarray) == false) {
      printf("Error parsing json object\n");
      return;
    }
  }

  int arraylen = json_object_array_length(jarray);
  int i;
  json_object *jvalue;

  for (i = 0; i < arraylen; i++) {
    jvalue = json_object_array_get_idx(jarray, i);
    type = json_object_get_type(jvalue);

    if (type == json_type_array) {
      json_parse_array(jvalue, NULL);
    } else if (type != json_type_object) {
    } else {
      json_parse(jvalue);
    }
  }
}

void json_parse(json_object *jobj) {
  enum json_type type;
  unsigned int count = 0;
  fade_energy_levels();  // drop LED value by a little each time you read a json for fade effect
  json_object_object_foreach(jobj, key, val) {
    type = json_object_get_type(val);
    switch (type) {
      case json_type_boolean:
        break;
      case json_type_double:
        if (!strcmp(key, "id")) {
          tracked_source_id = json_object_get_double(val);
        } else if (!strcmp(key, "x")) {
          x = json_object_get_double(val);
        } else if (!strcmp(key, "y")) {
          y = json_object_get_double(val);
        } else if (!strcmp(key, "z")) {
          z = json_object_get_double(val);
        } else if (!strcmp(key, "E")) {
          E = json_object_get_double(val);
        }
        // assign energy level for each potential source relative to its energy
        ++count;
          if (count == 4) {
//           printf ("count %d   x %f y %f E %f   -> id %d    \n",count,x,y,E,tracked_source_id);
           capture_energy_level_at_location();}
        break;
      case json_type_int:
        if (!strcmp(key, "id"))  tracked_source_id = json_object_get_int(val);
        break;
      case json_type_string:
        break;
      case json_type_object:
        if (json_object_object_get_ex(jobj, key, &jobj) == false) {
          printf("Error parsing json object\n");
          return;
        }
        json_parse(jobj);
        break;
      case json_type_array:
        json_parse_array(jobj, key);
        break;
    }
  }
}

int main(int argc, char *argv[]) {


// initialise arrays

  for (int i =0; i<LOCATIONS_COUNT;i++) {
       position_person_number[i]=-1;
      }

  for (int i =0; i<MAX_PARTICIPANTS;i++) {
       total_talk_time[i]=0;
    //  participant_is_talking[i]=false
            }

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

  char verbose = 0x00;

  int server_id;
  struct sockaddr_in server_address;
  int connection_id;
  char *message;
  int messageSize;

  int c;
  unsigned int portNumber = 9000;
  const unsigned int nBytes = 10240;

  server_id = socket(AF_INET, SOCK_STREAM, 0);

  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(portNumber);

  printf("Binding socket........... ");
  fflush(stdout);
  bind(server_id, (struct sockaddr *)&server_address, sizeof(server_address));
  printf("[OK]\n");

  printf("Listening socket......... ");
  fflush(stdout);
  listen(server_id, 1);
  printf("[OK]\n");

  printf("Waiting for connection in port %d ... ", portNumber);
  fflush(stdout);
  connection_id = accept(server_id, (struct sockaddr *)NULL, NULL);
  printf("[OK]\n");

  message = (char *)malloc(sizeof(char) * nBytes);

  printf("Receiving data........... \n\n");


  // this is where we put code to reset the pie for new meeting
  // either by pbutton press or by long silence

  while ((messageSize = recv(connection_id, message, nBytes, 0)) > 0) {
    message[messageSize] = 0x00;

    //printf("message: %s\n\n", message);
    json_object *jobj = json_tokener_parse(message);
    json_parse(jobj);

    // each time there is a json message recalc people and positions
    // using the energy array - the energy array is set by cyclng through
    // all the potential sources in json message

    for (int i = 0; i< LOCATIONS_COUNT; i++) {

    if  (energy_array[i] > MIN_THRESHOLD) {
        // someone is talking
        // if from a posiiton that is already assigned threads_single_open
        // increment the speak time for that person
        if (position_person_number[i]>-1) {
        total_talk_time[position_person_number[i]]++;
  //      participant_is_talking[position_person_number[i]] = true
        }
        else
        // else assign that area (+-10 degrees) to a new participant
        // and increment num_participants to reflect new person
        {

          for (int j=-2;j<3;j++) {
              int k = i+j;
              if (k<0) k+=LOCATIONS_COUNT;
              if (k>=LOCATIONS_COUNT) k-=LOCATIONS_COUNT;
              if (position_person_number[k] == -1) {
                position_person_number[k] = num_participants;
                              printf ("surrounding k %d \n", k);}
          ++num_participants;
         

        }
}
      }
    }


// re code this below
    for (int i = 0; i < bus.MatrixLeds(); i++) {
      // Convert from angle to pots index
      int index_pots = led_angles_in_matrixvoice[i] * LOCATIONS_COUNT / 360;
      // Mapping from pots values to color
      int brightness = energy_array[index_pots] * MAX_BRIGHTNESS / MAX_VALUE;
      // Removing colors below the threshold

      image1d.leds[i].red = brightness * participant_colour[position_person_number[index_pots]].red;
      image1d.leds[i].green = brightness * participant_colour[position_person_number[index_pots]].green;
      image1d.leds[i].blue = brightness * participant_colour[position_person_number[index_pots]].blue;
      image1d.leds[i].white = 0;
    }
    everloop.Write(&image1d);
  }
}
