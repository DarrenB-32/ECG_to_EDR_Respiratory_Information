
#include <SPI.h>
#include <WiFi.h>
#include <Wire.h>
#include <stdlib.h>
#include <limits.h>
WiFiClient client;

#include <cmsis_compiler.h>
#include <arm_math.h>

#define SIZE_BLOCKS  32
#define NUM_SAMPLES 500
#define NUMBER_OF_TAPS 4

char ssid[] = "NETGEAR75";    // your network SSID (name) 
char pass[] = "pastelsquirrel1"; // your network password (use for WPA, or use as key for WEP)

int Lo1 = 39;
int Lo2 = 40;

float32_t analog_data[NUM_SAMPLES]; // A global array for the input data
float32_t filtered_data[NUM_SAMPLES]; // A global array for the low-pass filtered output data
uint32_t Size_of_block = SIZE_BLOCKS;
uint32_t number_of_blocks = NUM_SAMPLES/SIZE_BLOCKS;



const float32_t Coefficients[NUMBER_OF_TAPS] = { 0.047, +0.4531f, +0.4531, +0.047f};
      float32_t States[NUMBER_OF_TAPS + SIZE_BLOCKS - 1];
arm_fir_instance_f32 S5;

char thingSpeakAddress[] = "api.thingspeak.com";
IPAddress hostIp(184,106,153,149);
String writeAPIKey = "7LJJE12AHI0MPB1Q";
const int updateThingSpeakInterval = 15 * 1000;

long lastConnectionTime = 0; 
boolean lastConnected = false;
const unsigned long postingInterval = 10*1000;


int analog_data1[NUM_SAMPLES]; // A global array
int qrs_area[NUM_SAMPLES];
// the setup routine runs once when you press reset:
void setup() {
  analogReadResolution(14); // Default is 10 bits; this switches to 14
   pinMode(Lo1, INPUT); // Setup for leads off detection LO +
  pinMode(Lo2, INPUT); // Setup for leads off detection LO -
  // initialize serial communications
  Serial.begin(57600); 
   WiFi.begin(ssid, pass);
  while ( WiFi.status() != WL_CONNECTED) {
    // print dots while we wait to connect
    Serial.print(".");
    delay(300);
  }
  
  Serial.println("\nYou're connected to the network");
  
  while (WiFi.localIP() == INADDR_NONE) {
    Serial.print(".");
    delay(300);
  }
  
  printWifiStatus();
}

int Q,R,R2,S,P,i,Q_index, R_index, threshold, S_index, Q1, Q2, S1, S2, Area1, Area2, Area3;
//unsigned int analog_data1[10] = {4,2,1,5,7,12,5,2,0,9};


void  WaveAnalysis(int *data, int & QRSArea, int &RR_interval, int &RS_slope, int & RS_distance){
    int R2 = 0;
    int threshold = 0;
    data[R]=0;
    int Q2 = 0;
    int Q1 = 0;
    int S1 = 0;
    int S2 = 0;
    

   int average=0;
  for(int a =0; a<NUM_SAMPLES; a++){
    average=average+data[a];
    }

    average = average/NUM_SAMPLES;

    Serial.print("Average value is: ");
    Serial.println(average);


    
    for( i=0; i<NUM_SAMPLES; i++){
      if(i!=0 && data[i]>data[R]){R=i;}// find biggest value = R
    }

    for( int d=R; d<NUM_SAMPLES; d+R2){///for loop through all samples
      
    Serial.print("R index ");
    Serial.println(R);

     Serial.print("R ");
    Serial.println((data[R]-average));

    for(int j = R ; j > 0 ; j--)
  {
    if(data[j-1] > data[j]) 
      { 
        Q = j;
        Serial.print("Q_index ");
        Serial.println(Q);
        break;
      }
  }

  Serial.print("Q ");
    Serial.println((data[Q]-average));

  for(int k = R ; k < NUM_SAMPLES ; k++)
  {
    if(data[k+1] > data[k]) 
      {
        S = k;
        Serial.print("S_index ");
        Serial.println(S);
        break;
      }
  }

   Serial.print("S ");
    Serial.println((data[S]-average));

    threshold = data[R]/10;

    Serial.print("Threshold is ");
    Serial.println(threshold);
    Serial.println("");
    Serial.println("");
//  
   for(int n=S; n<NUM_SAMPLES; n++)
   { 
      if(data[n]<=(data[R]+threshold) && data[n]>=(data[R]-threshold))
      {
        R2 = n;
        break;
      }
   }

   Serial.print("R2 index is ");
   Serial.println(R2);

   Serial.print("R2 value is ");
   Serial.println(data[R2]);

   RR_interval = (R2-R);
   RS_slope = (((data[S]-average) - (data[R]-average))/(S-R));
   RS_distance = S-R;

    Serial.print("RR_interval distance is ");
      Serial.println(RR_interval);
      Serial.print("RS_slope distance is ");
      Serial.println(RS_slope);
      Serial.print("RS_distance distance is ");
      Serial.println(RS_distance);
      Serial.println(""); 
      Serial.println("");

 for(int qa1=Q; qa1 > 0;qa1--)
 {
  if (data[qa1] > average -30)
  {
    Q1 = qa1;
    break;
  }
 }

 for(int qa2=Q; qa2 < NUM_SAMPLES ;qa2++)
 {
  if (data[qa2] > average -30)
  {
    Q2 = qa2;
    break;
  }
 }

for(int sa1=S; sa1 > 0; sa1--)
 {
  if (data[sa1] > average -30)
  {
    S1 = sa1;
    break;
  }
 }

 for(int sa2=S; sa2 < NUM_SAMPLES ;sa2++)
 {
  if (data[sa2] > average -30)
  {
    S2 = sa2;
    break;
  }
 }


   
  Area1=(abs(S2-S1)/2)*((data[S1]-average)-(data[S]-average));
  Area2=(abs(Q2-Q1)/2)*((data[Q2]-average)-(data[Q]-average));
  Area3=(abs(S1-Q2)/2)*((data[R]-average)-(data[Q1]-average));


  Serial.print("Q1 index is ");
  Serial.println(Q1);

  Serial.print("Q1 point is ");
  Serial.println((data[Q1]-average));

    Serial.print("Q2 index is ");
  Serial.println(Q2);

    Serial.print("Q2 point is ");
  Serial.println((data[Q2]-average));

    Serial.print("S1 index is ");
  Serial.println(S1);

  Serial.print("S1 point is ");
  Serial.println((data[S1]-average));

    Serial.print("S2 index is ");
  Serial.println(S2);

  Serial.print("S2 point is ");
  Serial.println((data[S2]-average));

    Serial.print("Area1 point is ");
  Serial.println(Area1);

    Serial.print("Area2 point is ");
  Serial.println(Area2);

    Serial.print("Area3 point is ");
  Serial.println(Area3);


   QRSArea = Area1+Area2+Area3;

    Serial.print("QRS Area is ");
  Serial.println(QRSArea);


    qrs_area[R]=QRSArea;
    
    R=R2;}//for loop through all samples

}


// the loop routine runs over and over again forever:
void loop() {
  unsigned long startTime;
  unsigned int elapsedTime;
  startTime = micros(); // number of microseconds since board began running the current program
  
  for(int count=0; count < NUM_SAMPLES; count++) {  // count only available within the for loop brackets
    analog_data1[count] = analogRead(A14)*(3.3/1023);
    delay(1); // 1ms delay in between reads for stability - investigate if this is necessary
    }
  elapsedTime = micros() - startTime;   
  int QRS_AREA, RR_interval, RS_slope, RS_distance;
  WaveAnalysis(analog_data1,QRS_AREA, RR_interval, RS_slope, RS_distance);
  Serial.print("QRS Area in loop is ");
  Serial.println(QRS_AREA);
  Serial.println(RR_interval);
  Serial.println(RS_slope);
  Serial.println(RS_distance);
  Serial.println(""); 
  Serial.println("");
  for(int count=0; count < NUM_SAMPLES; count++) 
  {
      Serial.println(qrs_area[count]);
  }

  float32_t  *input, *output; 
   output = &filtered_data[0];       
  input  = &analog_data[0];
 
  
  // Call FIR init function. 
  arm_fir_init_f32(&S5, NUMBER_OF_TAPS, (float32_t *)&Coefficients[0], &States[0], SIZE_BLOCKS);

  for(int i=0; i < number_of_blocks; i++)
  {
    arm_fir_f32(&S5, input + (i * Size_of_block), output + (i * Size_of_block), Size_of_block);
  }

  for (int count = 0; count < NUM_SAMPLES; count++) {
    Serial.print(analog_data[count],6);
    Serial.print("\t");
    Serial.println(filtered_data[count],6);
  }
  delay(10000);  
   Serial.println(""); 
   Serial.println("");
   
   client.stop();
  String QRS_AREA_String = String(QRS_AREA);
  String RR_interval_String = String(RR_interval);
  String RS_slope_String = String(RS_slope);
  String RS_distance_String = String(RS_distance);
  if(!client.connected() && (millis() - lastConnectionTime > updateThingSpeakInterval))
  {
   // updateThingSpeak("field1="+analogPin0+"&field2="+digitalPin);
   updateThingSpeak("field1="+QRS_AREA_String+"&field2="+RR_interval_String+"&field3="+RS_slope_String+"&field4="+RS_distance_String);
  }
  
  lastConnected = client.connected();
 delay(15000);
}


void updateThingSpeak(String DataThingsSpeak)
{
  Serial.println("Updating ThingSpeak");
  
  if (client.connect(hostIp, 80)) {
    Serial.println("Connected to ThingSpeak!");
    Serial.println();
    
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+writeAPIKey+"\n");
    client.print("Content-Length: ");
    client.print(DataThingsSpeak.length());
    client.print("\n\n");
    client.print(DataThingsSpeak);
    
    Serial.println("ThingSpeak has been updated with new values!");
  } else {
    client.stop();
  }  
  lastConnectionTime = millis();
}

void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ipaddr = WiFi.localIP();
  Serial.print("IP Address of SSID: ");
  Serial.println(ipaddr);
  delay(1000);
}
