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

#define ANGLE_SPREAD 25.0  // Gives 30 degree window for each speaker
// MAX_VALUE : max value of energy
#define MIN_THRESHOLD 50
// MAX_BRIGHTNESS: 0 - 20
#define MAX_BRIGHTNESS 12
// MAX_PARTICIPANTS: Max number of people in meeting
#define MAX_PARTICIPANTS 6

//these variables hold the value from odas of x,y and energy_array
double x, y, z, E;
int tracked_source_id;

struct meeting_member {
  int led_num;
  double angle;
  bool talking; 
  bool was_talking; 
  long total_talk_time;
  char sex;
  int num_turns;};
  
  struct meeting_member participant [MAX_PARTICIPANTS];

long total_meeting_duration=0;
int num_participants = 0;

struct rgb_value {int red; int green; int blue;} ;
struct rgb_value colour[MAX_PARTICIPANTS] = {
  {12,0,0},
  {0,12,0},
  {0,0,9},
  {6,6,0},
  {6,0,5},
  {0,6,5}};

/*
const double led_angles_in_matrixvoice[18] = {170, 150, 130, 110, 90,  70,
                                      50,  30,  10,  350, 330, 310,
                                      290, 270, 250, 230, 210, 190};
*/



void capture_energy_level_at_location() {

  int num_matches=0;
  int person_speaking=-1;
  // Convert x,y to angle. TODO: See why x axis from ODAS is inverted
  double angle_xy = (atan2(y, x) * (180.0 / M_PI)) ;
  double matched_angle_diff, angle_diff;
  //check is angle already recorded
  for (int i=0;i<num_participants;i++) {
  // check how far away it is from a known source
    angle_diff = participant[i].angle - angle_xy;
    angle_diff += (angle_diff>180.0) ? -360.0 : (angle_diff<-180.0) ? 360.0 : 0.0;
    if (abs(angle_diff) < ANGLE_SPREAD) {
      ++num_matches;
      person_speaking = i;
      matched_angle_diff = angle_diff;}
      // this can be simplified and optimised by exiting loop when match is found
      
}
// end of checking through existing

   if (num_matches == 0) {
// new participant
     participant[num_participants].angle = angle_xy;
     participant[num_participants].led_num = (180-angle_xy)/20;
     participant[num_participants].talking = true;
     participant[num_participants].num_turns = 1;
     participant[num_participants].total_talk_time = 0;
     printf ("new member %d at %3.2f degrees.  LED number: %d \n", num_participants,angle_xy, participant[num_participants].led_num);
     if (num_participants<MAX_PARTICIPANTS-1) {
     ++num_participants;}
     else{
     printf("error too many members");
     }
     
    };    

   if (num_matches == 1) {
// add to existing participant
     participant[person_speaking].talking = true;
     if (!participant[person_speaking].was_talking) {
     ++participant[person_speaking].num_turns;
     participant[person_speaking].angle += matched_angle_diff / participant[person_speaking].num_turns;
     printf ("Angle diff = %3.2f.  Person %d is now %3.2f and they talked for %ld\n", matched_angle_diff, person_speaking, participant[person_speaking].angle, participant[person_speaking].total_talk_time);
     }
     ++participant[person_speaking].total_talk_time;
    };    
   if (num_matches > 1) printf ("error overlap of %d people\n", num_matches);
}


void json_parse_array(json_object *jobj, char *key) {
  // Forward Declaration
  void json_parse(json_object * jobj);
  enum json_type type;
//  printf("have gone into parse array!!\n");

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
          if (count == 4 and E > 0.5) {
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

  int server_id;
  struct sockaddr_in server_address;
  int connection_id;
  char *message;
  int messageSize;
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
  fflush(stdout);
  connection_id = accept(server_id, (struct sockaddr *)NULL, NULL);
  message = (char *)malloc(sizeof(char) * nBytes);

  printf("Receiving data........... \n\n");

  while ((messageSize = recv(connection_id, message, nBytes, 0)) > 0) {
    message[messageSize] = 0x00;

    //printf("message: %s\n\n", message);

// reset all particpants to be silent - but record if they were talking in last message set
  for (int i=0;i<num_participants;i++) {
     participant[i].was_talking = participant[i].talking ;  // store value  
     participant[i].talking = false; // then reset
   }

    json_object *jobj = json_tokener_parse(message);
    json_parse(jobj);

    // each time there is a json message recalc people and positions
    // using the energy array - the energy array is set by cyclng through
    // all the potential sources in json message
      

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
