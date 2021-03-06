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
#define INCREMENT 20
// MIN_THRESHOLD: Filters out low energy targets from odas
#define MIN_THRESHOLD 12
// MAX_BRIGHTNESS: 0 - 20
#define MAX_BRIGHTNESS 12
// MAX_PARTICIPANTS: Max number of people in meeting
#define MAX_PARTICIPANTS 9

const int led_mapping[54] = {8,8,8,9,9,9,10,10,10,11,11,11,12,12,12,13,13,13,14,14,14,15,15,15,
  16,16,16,17,17,17,0,0,0,1,1,1,2,2,2,3,3,4,4,4,5,5,5,6,6,6,7,7,7};

//these variables hold the value from odas of x,y and energy_array
double x, y, z, E;

int position_person_number[LOCATIONS_COUNT];
int total_talk_time[MAX_PARTICIPANTS];
int num_participants = 0;
//bool participant_is_talking[MAX_PARTICIPANTS];
int energy_array[LOCATIONS_COUNT];
int led_number[MAX_PARTICIPANTS];

struct led_values {int number; int energy;int red; int green; int blue; };
struct led_values led[MAX_PARTICIPANTS]= {
  {0,0,12,0,0},
  {0,0,0,12,0},
  {0,0,0,0,12},
  {0,0,6,6,0},
  {0,0,6,0,6},
  {0,0,0,6,6},
  {0,0,4,4,2},
  {0,0,4,2,4},
  {0,0,2,4,4} };


const double led_angles_in_matrixvoice[18] = {170, 150, 130, 110, 90,  70,
                                      50,  30,  10,  350, 330, 310,
                                      290, 270, 250, 230, 210, 190};

void capture_energy_level_at_location() {
  // Convert x,y to angle. TODO: See why x axis from ODAS is inverted
  double angle_xy = fmodf((atan2(y, x) * (180.0 / M_PI)) + 360, 360);
  // Convert angle to index
  int i_angle = angle_xy / 360 * LOCATIONS_COUNT;  // convert degrees to index
  // Set energy for this angle
  energy_array[i_angle] = INCREMENT * E;
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
  json_object_object_foreach(jobj, key, val) {
    type = json_object_get_type(val);
    switch (type) {
      case json_type_boolean:
        break;
      case json_type_double:
        if (!strcmp(key, "x")) {
          x = json_object_get_double(val);
        } else if (!strcmp(key, "y")) {
          y = json_object_get_double(val);
        } else if (!strcmp(key, "z")) {
          z = json_object_get_double(val);
        } else if (!strcmp(key, "E")) {
          E = json_object_get_double(val);
        }
        // assign energy level for each potential source relative to its energy
        capture_energy_level_at_location();
        count++;
        break;
      case json_type_int:
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
// 9001 is for pots
// 9000 is for targets
  unsigned int portNumber = 9000;
  const unsigned int nBytes = 10240;

  server_id = socket(AF_INET, SOCK_STREAM, 0);

  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(portNumber);
  fflush(stdout);
  bind(server_id, (struct sockaddr *)&server_address, sizeof(server_address));
  fflush(stdout);
  listen(server_id, 1);
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

// reset all energuies to zero
    for (int i = 0; i< LOCATIONS_COUNT; i++) energy_array[i] = 0;
    for (int i = 0; i< num_participants; i++) led[position_person_number[i]].energy = 0;

//    printf("message: %s\n\n", message);
    json_object *jobj = json_tokener_parse(message);
    json_parse(jobj);

    // each time there is a json message recalc people and positions
    // using the energy array - the energy array is set by cyclng through
    // all the potential sources in json message

    for (int i = 0; i< LOCATIONS_COUNT; i++) {

    if  (energy_array[i] > MIN_THRESHOLD) {
        if (position_person_number[i]>-1) {
        total_talk_time[position_person_number[i]]++;
        led[position_person_number[i]].energy = energy_array[i];
        led[position_person_number[i]].number = led_mapping[i];      
  //      participant_is_talking[position_person_number[i]] = true
  //      printf ("person %d talking total %d \n", position_person_number[i],total_talk_time[position_person_number[i]]);
        }
        else
        // else assign that area (+-10 degrees) to a new participant
        // and increment num_participants to reflect new person
        {++num_participants;
          for (int j=-2;j<3;j++) {
              int k = i+j;
              if (k<0) k+=LOCATIONS_COUNT;
              if (k>=LOCATIONS_COUNT) k-=LOCATIONS_COUNT;
              if (position_person_number[k] == -1) {
                position_person_number[k] = num_participants;
                              printf ("surrounding k %d \n", k);}
          }

        }

      }
    }


// re code this below
    for (int i = 1; i <= num_participants; i++) {
      // Convert from angle to pots index

      image1d.leds[led[i].number].red = led[i].energy * led[i].red;
      image1d.leds[led[i].number].green = led[i].energy * led[i].green;
      image1d.leds[led[i].number].blue = led[i].energy * led[i].blue;
      image1d.leds[led[i].number].white = 0;
    }
    everloop.Write(&image1d);
  }
}
