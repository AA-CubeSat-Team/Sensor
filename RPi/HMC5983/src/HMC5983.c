#include "HMC5983.h"
#include "../lib/I2Cdrive.h"
#include "../lib/commonLib.h"

//const char* csv_filename = "aerb_cal.csv";
//const char* csv_filename = "cal_val.csv";
static const char* csv_filename = "sat_cave_cal.csv";


static struct hmc5983_Var3 mag_offset = {0, 0,0};
static struct hmc5983_Var3 mag_scale = {1, 1, 1}; 

// The gain setting value with the following field range
//    0x00: .88 Gauss
//    0x20: 1.3 Gauss
//    0x40: 1.9 Gauss
//    0x60: 2.5 Gauss
//    0x80: 4.0 Gauss
//    0xA0: 4.7 Gauss
//    0xC0: 5.6 Gauss
//    0xE0: 8.1 Gauss 


/*
 Convert byte value for X and Y direction
 to the appropriate Magnetic field value 

    Parameters:: val: the hex value from the sensor
    Returns:: field: The magnetic field value 
 */
static float hmc5983_convert_hex_2_field(int16_t val){
  float field = 0.0;
  unsigned char gainVal =  hmc5983_get_crb();
  if(gainVal == HMC5983Gain08)
    field = val/1370.0;
  else if(gainVal == HMC5983Gain13)
    field = val/1090.0;
  else if(gainVal == HMC5983Gain19)
    field = val/820.0;
  else if(gainVal ==HMC5983Gain25)
    field = val/660.0;
  else if(gainVal == HMC5983Gain40)
    field = val/440.0;
  else if(gainVal == HMC5983Gain47)
    field = val/390.0;
  else if(gainVal == HMC5983Gain56)
    field = val/330.0;
  else
    field = val/230.0;
  return field;
}

/*
  Connect to the HMC5983 chip using I2C

  Returns:: 1 if unable to connect, else 0
 */
int connect_2_hmc5983_via_I2C(){
  //int slaveAddr = 0x1E;
  //mag_offset = {0.0, 0.0, 0.0};
  //mag_scale = {1.0, 1.0, 1.0};
  return i2c_connect_2_slave(HMC5983Address);
}

/*
 Set the CRA register on the device. This could 
enable/disable temperature, the data output rate,
and measurement mode.

Parameters:: craSetting: Refer to Table 3 - 6 for cra register setting
Returns:: 0 if success, 1 if failed.
 */
int hmc5983_set_cra(unsigned char craSetting){
  int i = 0;
  while(i2c_set_register_val(HMC5983ConfRegA, craSetting) && i < 10){
    printf("Error in setting CRA REG. Trying again\n");
    i += 1;
  }
  if (i == 10){
    printf("Could not set the CRA reg\n");
    return 1;
  }
  return 0;
}

/*
 Get the CRA register value, which will tell you
if temperature sensor is enabled, the data rate, 
and the measurement mode. 

Parameters:: None
Returns:: The cra register value

 */
unsigned char hmc5983_get_cra(){
  int i  = i2c_get_register_val(HMC5983ConfRegA);
  unsigned char res = i2c_get_buffer_index(i);
  //printf("The current cra valus is 0x%02x\n", res);
  return res;
}


/*
 Get the mode of the HMC5983

Parameters:: None
Returns:: The value of the configuration register b. Refer
   to Table 10-12 for the meaning of the value.
 */
unsigned char hmc5983_get_mode(){
  int i = i2c_get_register_val(HMC5983ModeReg);
  unsigned char res = i2c_get_buffer_index(i);
  //printf("The current mode is 0x%02x\n", res);
  return res;
}

/*
The mode determines if the device should sleep, continuous 
get data, or get data when asked. Note that when you set to 
single conversion mode (0x01), then once the device get the 
measurement, it will set itself to idle mode (0x03)

mode: set devMode to the following
      0x00 for continuous conversion mode
      0x01 for single conversion mode
      0x02 or 0x03 for sleep mode

Parameters:: deviceMode: the hex value which represents which
     mode you want the device to be in
Returns:: 0 if succeds, else 1
 */
int hmc5983_set_mode(unsigned char deviceMode){
  int i = 0;
  while(i2c_set_register_val(HMC5983ModeReg, deviceMode) && i < 10){
    printf("Error in setting mode reg. Trying again\n");
    i += 1;
  }
  if (i == 10){
    printf("Could not set mode register.\n");
    return 1;
  }
  return 0;
}

/*
  Get the gain setting. The different 
  values range from 0x00-0xE0. Refer to 
  Table 9 in the datasheet for details.
  
  Parameters:: None
  Returns:: The gain setting of the device.

 */
unsigned char hmc5983_get_crb(){
  int i = i2c_get_register_val(HMC5983ConfRegB);
  unsigned char res = i2c_get_buffer_index(i);
  return res;
}

/*
  Set the gain of the device.Refer to Table 9 in 
  the datasheet for details.


  Parameters:: gainSetting: The hex value that represents \
                            the gain value you want to set.
  Returns:: The gain setting of the device. 
 */
int hmc5983_set_crb(unsigned char gainSetting){
  int i = 0;
  while(i2c_set_register_val(HMC5983ConfRegB, gainSetting) && i < 10){
    printf("Error in setting gain register. Trying again\n");
    i += 1;
  }
  if(i == 10){
    printf("Could not set gain register\n");
    return 1;
  }
  return 0;
}


/*
 Get the raw magnetic field in the X-direction of the 
 device in Gauss

 Parameters:: apply_gain: convert value to Gauss based on gain
 */
float hmc5983_get_raw_magnetic_x(unsigned char apply_gain){
  int firstInd = i2c_get_register_val(HMC5983XMSB);
  int secondInd = i2c_get_register_val(HMC5983XLSB);

  if(firstInd == -1 || secondInd == -1)
    return NAN;

  unsigned char firstByte = i2c_get_buffer_index(firstInd);
  unsigned char secondByte = i2c_get_buffer_index(secondInd); 
  //printf("Data recieved for Mag X is 0x%02x%02x\n", firstByte, secondByte);

  float fieldVal= (firstByte << 8| (secondByte));
  if(apply_gain){
    fieldVal = hmc5983_convert_hex_2_field(fieldVal);
  }
  return fieldVal;
}

/*
 Get the calibrated magnetic field in the X-direction of the 
 device in Gauss
 */
float hmc5983_get_magnetic_x(){
  float fieldVal = hmc5983_get_raw_magnetic_x(1);
  if(isnan(fieldVal))
    return NAN;
  fieldVal = (fieldVal - mag_offset.x) * mag_scale.x;
  return fieldVal;
}

/*
 Get the raw magnetic field in the Y-direction of the 
 device in Gauss

 Parameters:: apply_gain: if 1 convert value to Gauss based on gain
 */
float hmc5983_get_raw_magnetic_y(unsigned char apply_gain){
  int firstInd = i2c_get_register_val(HMC5983YMSB);
  int secondInd = i2c_get_register_val(HMC5983YLSB);

  if(firstInd == -1 || secondInd == -1)
    return NAN;

  unsigned char firstByte = i2c_get_buffer_index(firstInd);
  unsigned char secondByte = i2c_get_buffer_index(secondInd); 
  //printf("Data recieved for Mag Y is 0x%02x%02x\n", firstByte, secondByte);

  float fieldVal= (firstByte << 8| (secondByte));
  if(apply_gain){
    fieldVal = hmc5983_convert_hex_2_field(fieldVal);
  }
  return fieldVal;
}

/*
 Get the calibrated magnetic field in the Y-direction of the 
 device in Gauss
 */
float hmc5983_get_magnetic_y(){
  float fieldVal = hmc5983_get_raw_magnetic_y(1);
  if (isnan(fieldVal))
    return NAN;
  fieldVal = (fieldVal - mag_offset.y) * mag_scale.y;
  return fieldVal;
}

/*
 Get the raw magnetic field in the Z-direction of the 
 device in Gauss

 Parameters:: apply_gain: if 1 convert value to Gauss based on gain
 */
float hmc5983_get_raw_magnetic_z(unsigned char apply_gain){
  int firstInd = i2c_get_register_val(HMC5983ZMSB);
  int secondInd = i2c_get_register_val(HMC5983ZLSB);

  if(firstInd == -1 || secondInd == -1)
    return NAN;

  unsigned char firstByte = i2c_get_buffer_index(firstInd);
  unsigned char secondByte = i2c_get_buffer_index(secondInd); 
  //printf("Data recieved for Mag Z is 0x%02x%02x\n", firstByte, secondByte);


  float fieldVal= (firstByte << 8| (secondByte));
  if(apply_gain){
    fieldVal = hmc5983_convert_hex_2_field(fieldVal);
  }
  return fieldVal;
}

/*
 Get the calibrated magnetic field in the Z-direction of the 
 device in Gauss
 */
float hmc5983_get_magnetic_z(){
  float fieldVal = hmc5983_get_raw_magnetic_z(1);
  if(isnan(fieldVal))
    return NAN;
  fieldVal = (fieldVal - mag_offset.z ) * mag_scale.z;
  return fieldVal;
}

/*
  Get the device's status. Only the first 2 bits matters.
  The first bit represents if the new set of 
  measurement is available. The second bit tells you if the 
  measurement is locked. 
 */
int hmc5983_get_status(){
  int i = i2c_get_register_val(HMC5983STATUS);
  unsigned char res = i2c_get_buffer_index(i);
  //printf("The current status is 0x%02x\n", res);
  return res;
}


float hmc5983_get_temp(){
  int i1 = i2c_get_register_val(HMC5983TempMSB);
  int i2 = i2c_get_register_val(HMC5983TempLSB);

  int16_t tempMSB = i2c_get_buffer_index(i1);
  int16_t tempLSB = i2c_get_buffer_index(i2);  
  
  //printf("%02x\n", tempMSB);
  //printf("%02x\n", tempLSB);
  float temp = (((tempMSB << 8) | tempLSB) >> 4)/8.0 + 25.0;
  return temp;
}

/*
  Calibrate the magnetometer result. This will require moving 
the magnetometer in a random motion such that it'll know the range
of data.
 */
void hmc5983_calibrate(){
  // Calibration step. Record data and get the biases.
  printf("\n\n\n***** In the calibration step ******\n");
  printf("Move the magnetometer in a figure 8 motion while calibrating\n");
  float low_val[3] = {INFINITY, INFINITY, INFINITY};    // {x, y, z}
  float high_val[3] = {-INFINITY, -INFINITY, -INFINITY};  // {x, y, z}

  float x_m =0.0;
  float y_m = 0.0;
  float z_m = 0.0;
  //float temper = 0.0;
  int i = 0;
  while(i < 1000){
    if(i%100 == 0)
      printf("Iteration %d\n", i);
    x_m = hmc5983_get_raw_magnetic_x(1);   
    //printf("The magnetic field in X is %f Gauss\n", x_m);
  
    y_m = hmc5983_get_raw_magnetic_y(1);
    //printf("The magnetic field in Y is %f Gauss\n", y_m);
  
    z_m = hmc5983_get_raw_magnetic_z(1);
    //printf("The magnetic field in Z is %f Gauss\n", z_m);

    if(isnan(x_m) || isnan(y_m) || isnan(z_m)){
      continue;
    }
    
    if(low_val[0] > x_m){
      low_val[0] = x_m;
    }else if(high_val[0] <= x_m){
      high_val[0] = x_m;
    }

    if(low_val[1] > y_m){
      low_val[1] = y_m;
    }else if(high_val[1] <= y_m){
      high_val[1] = y_m;
    }

    if(low_val[2] > z_m){
      low_val[2] = z_m;
    }else if(high_val[2] <= z_m){
      high_val[2] = z_m;
    }
    
    //temper = hmc5983_get_temp();
    usleep(30000);
    i++;
  }
  
  mag_offset.x = (high_val[0] + low_val[0])/2;
  mag_offset.y = (high_val[1] + low_val[1])/2;
  mag_offset.z = (high_val[2] + low_val[2])/2;

  // Soft iron calibration
  float avg_delta_xyz[3] = {(high_val[0] - low_val[0])/2, (high_val[1] - low_val[1])/2, (high_val[2] - low_val[2])/2};
  float avg_delta = (avg_delta_xyz[0] + avg_delta_xyz[1] + avg_delta_xyz[2])/3;

  mag_scale.x = avg_delta/ avg_delta_xyz[0];
  mag_scale.y = avg_delta/ avg_delta_xyz[1];
  mag_scale.z = avg_delta/ avg_delta_xyz[2];
  printf("The magnetic offset are %.3f %.3f %.3f\n", mag_offset.x, mag_offset.y , mag_offset.z);

  FILE *fp;
  //char buff[255];
  fp = fopen(csv_filename, "w");

  fprintf(fp, "%f", mag_offset.x);
  fprintf(fp, ",%f", mag_offset.y);
  fprintf(fp, ",%f\n", mag_offset.z);

  fprintf(fp, "%f", mag_scale.x);
  fprintf(fp, ",%f", mag_scale.y);
  fprintf(fp, ",%f\n", mag_scale.z);
  
  
  fclose(fp);
  
  printf("***** End of calibration *****\n\n\n");
}

void hmc5983_get_calibrated_value_from_file(){
  FILE *fp;
  int len = 4098;
  char buff[len];
  fp = fopen(csv_filename, "r");

  printf("Getting offset and scale from file\n");
  
  fgets(buff, len, fp);
  char* field = strtok(buff, ",");
  mag_offset.x = atof(field);

  field = strtok(NULL, ",");
  mag_offset.y = atof(field);
  
  field = strtok(NULL,  ",");
  mag_offset.z = atof(field);

  fgets(buff, len, fp);
  field = strtok(buff, ",");
  mag_scale.x = atof(field);

  field = strtok(NULL, ",");
  mag_scale.y = atof(field);
  
  field = strtok(NULL,  ",");
  mag_scale.z = atof(field);
  
  printf("mag offset are %.2f, %.2f, %.2f\n", mag_offset.x, mag_offset.y, mag_offset.z);
    printf("mag scale are %.2f, %.2f, %.2f\n", mag_scale.x, mag_scale.y, mag_scale.z);

  
  fclose(fp);
}

void hmc5983_set_calibration_offset(float offset_x, float offset_y, float offset_z){
  mag_offset.x = offset_x;
  mag_offset.y = offset_y;
  mag_offset.z = offset_z;
  
}

void hmc5983_set_calibration_scale( float magscale_x, float magscale_y, float magscale_z){
  mag_scale.x = magscale_x;
  mag_scale.y = magscale_y;
  mag_scale.z = magscale_z;
  
}
