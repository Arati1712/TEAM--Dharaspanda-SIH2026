#include <Wire.h>

// ============================================================
//        ESP32 + ADXL345 + MPU6050 + SOIL MOISTURE
//        MineGuard Monitoring System
//        Reading Interval = 5 Seconds
// ============================================================


// ============================================================
// I2C PINS
// ============================================================

#define SDA_PIN 21
#define SCL_PIN 22


// ============================================================
// ADXL345
// ============================================================

#define ADXL345_ADDR 0x53

#define ADXL_DEVID       0x00
#define ADXL_POWER_CTL   0x2D
#define ADXL_DATA_FORMAT 0x31
#define ADXL_DATAX0      0x32


// ============================================================
// MPU6050
// ============================================================

#define MPU6050_ADDR 0x68

#define MPU_WHO_AM_I      0x75
#define MPU_PWR_MGMT1     0x6B
#define MPU_ACCEL_XOUT_H  0x3B
#define MPU_GYRO_XOUT_H   0x43
#define MPU_ACCEL_CONFIG  0x1C
#define MPU_GYRO_CONFIG   0x1B


// ============================================================
// SOIL MOISTURE SENSOR
// ============================================================

#define SOIL_PIN 34

// Calibration values
// Dry soil  -> approximately 3000
// Wet soil  -> approximately 1200

#define SOIL_DRY_VALUE 3000
#define SOIL_WET_VALUE 1200


// ============================================================
// READING INTERVAL
// ============================================================

const unsigned long READING_INTERVAL = 5000;

unsigned long lastReadingTime = 0;


// ============================================================
// FUNCTION DECLARATIONS
// ============================================================

bool initializeADXL345();

bool initializeMPU6050();

void readAndDisplaySensors();

void readADXL345(
  int16_t &x,
  int16_t &y,
  int16_t &z
);

void readMPU6050(
  int16_t &ax,
  int16_t &ay,
  int16_t &az,
  int16_t &gx,
  int16_t &gy,
  int16_t &gz
);

void writeRegister(
  byte deviceAddress,
  byte reg,
  byte value
);

byte readRegister(
  byte deviceAddress,
  byte reg
);

void printFloat(
  float value,
  int decimals
);


// ============================================================
// SETUP
// ============================================================

void setup()
{
  Serial.begin(115200);

  delay(1000);


  // ----------------------------------------------------------
  // Start I2C
  // ----------------------------------------------------------

  Wire.begin(
    SDA_PIN,
    SCL_PIN
  );

  Wire.setClock(100000);


  // ----------------------------------------------------------
  // ESP32 ADC
  // ----------------------------------------------------------

  analogReadResolution(12);


  // ==========================================================
  // MAIN HEADER
  // ==========================================================

  Serial.println();
  Serial.println("============================================================");
  Serial.println("           MINEGUARD MONITORING PROTOTYPE");
  Serial.println("============================================================");

  Serial.println("System Initializing...");
  Serial.println();


  // ==========================================================
  // INITIALIZE ADXL345
  // ==========================================================

  if (initializeADXL345())
  {
    Serial.println("ADXL345 : CONNECTED");
  }
  else
  {
    Serial.println("ADXL345 : NOT FOUND");
  }


  // ==========================================================
  // INITIALIZE MPU6050
  // ==========================================================

  if (initializeMPU6050())
  {
    Serial.println("MPU6050 : CONNECTED");
  }
  else
  {
    Serial.println("MPU6050 : NOT FOUND");
  }


  // ==========================================================
  // SOIL SENSOR
  // ==========================================================

  Serial.println("SOIL SENSOR : READY");


  // ==========================================================
  // SYSTEM INFORMATION
  // ==========================================================

  Serial.println();
  Serial.println("------------------------------------------------------------");

  Serial.println("I2C SDA             : GPIO 21");
  Serial.println("I2C SCL             : GPIO 22");
  Serial.println("ADXL345 ADDRESS     : 0x53");
  Serial.println("MPU6050 ADDRESS     : 0x68");
  Serial.println("SOIL SENSOR PIN     : GPIO 34");
  Serial.println("READING INTERVAL    : 5 SECONDS");

  Serial.println("------------------------------------------------------------");

  Serial.println();
  Serial.println("System Ready.");
  Serial.println("============================================================");

  delay(1000);
}


// ============================================================
// LOOP
// ============================================================

void loop()
{
  if (
    millis() - lastReadingTime >=
    READING_INTERVAL
  )
  {
    lastReadingTime = millis();

    readAndDisplaySensors();
  }
}


// ============================================================
// INITIALIZE ADXL345
// ============================================================

bool initializeADXL345()
{
  Wire.beginTransmission(
    ADXL345_ADDR
  );

  byte error =
    Wire.endTransmission();

  if (
    error != 0
  )
  {
    return false;
  }


  // ----------------------------------------------------------
  // Read Device ID
  // ----------------------------------------------------------

  byte deviceID =
    readRegister(
      ADXL345_ADDR,
      ADXL_DEVID
    );


  if (
    deviceID != 0xE5
  )
  {
    return false;
  }


  // ----------------------------------------------------------
  // Measurement mode
  // ----------------------------------------------------------

  writeRegister(
    ADXL345_ADDR,
    ADXL_POWER_CTL,
    0x08
  );


  // ----------------------------------------------------------
  // Full resolution
  // +/-2g range
  // ----------------------------------------------------------

  writeRegister(
    ADXL345_ADDR,
    ADXL_DATA_FORMAT,
    0x08
  );


  delay(100);

  return true;
}


// ============================================================
// INITIALIZE MPU6050
// ============================================================

bool initializeMPU6050()
{
  Wire.beginTransmission(
    MPU6050_ADDR
  );

  byte error =
    Wire.endTransmission();

  if (
    error != 0
  )
  {
    return false;
  }


  // ----------------------------------------------------------
  // Read WHO_AM_I
  // ----------------------------------------------------------

  byte deviceID =
    readRegister(
      MPU6050_ADDR,
      MPU_WHO_AM_I
    );


  if (
    deviceID != 0x68
  )
  {
    return false;
  }


  // ----------------------------------------------------------
  // Wake up MPU6050
  // ----------------------------------------------------------

  writeRegister(
    MPU6050_ADDR,
    MPU_PWR_MGMT1,
    0x00
  );


  // ----------------------------------------------------------
  // Accelerometer +/-2g
  // ----------------------------------------------------------

  writeRegister(
    MPU6050_ADDR,
    MPU_ACCEL_CONFIG,
    0x00
  );


  // ----------------------------------------------------------
  // Gyroscope +/-250 deg/s
  // ----------------------------------------------------------

  writeRegister(
    MPU6050_ADDR,
    MPU_GYRO_CONFIG,
    0x00
  );


  delay(100);

  return true;
}


// ============================================================
// READ ALL SENSORS
// ============================================================

void readAndDisplaySensors()
{
  // ==========================================================
  // VARIABLES
  // ==========================================================

  int16_t adxlX = 0;
  int16_t adxlY = 0;
  int16_t adxlZ = 0;


  int16_t mpuAccelX = 0;
  int16_t mpuAccelY = 0;
  int16_t mpuAccelZ = 0;


  int16_t mpuGyroX = 0;
  int16_t mpuGyroY = 0;
  int16_t mpuGyroZ = 0;


  // ==========================================================
  // ADXL345 READING
  // ==========================================================

  readADXL345(
    adxlX,
    adxlY,
    adxlZ
  );


  // Convert ADXL raw to g

  float adxlXg =
    adxlX * 0.0039;

  float adxlYg =
    adxlY * 0.0039;

  float adxlZg =
    adxlZ * 0.0039;


  // ==========================================================
  // MPU6050 READING
  // ==========================================================

  readMPU6050(
    mpuAccelX,
    mpuAccelY,
    mpuAccelZ,
    mpuGyroX,
    mpuGyroY,
    mpuGyroZ
  );


  // ----------------------------------------------------------
  // MPU6050 acceleration
  // +/-2g -> 16384 LSB/g
  // ----------------------------------------------------------

  float mpuAccelXg =
    mpuAccelX / 16384.0;

  float mpuAccelYg =
    mpuAccelY / 16384.0;

  float mpuAccelZg =
    mpuAccelZ / 16384.0;


  // ----------------------------------------------------------
  // MPU6050 gyroscope
  // +/-250 deg/s -> 131 LSB/(deg/s)
  // ----------------------------------------------------------

  float gyroX =
    mpuGyroX / 131.0;

  float gyroY =
    mpuGyroY / 131.0;

  float gyroZ =
    mpuGyroZ / 131.0;


  // ==========================================================
  // SOIL MOISTURE
  // ==========================================================

  int soilRaw =
    analogRead(
      SOIL_PIN
    );


  // ----------------------------------------------------------
  // Convert ADC to percentage
  // ----------------------------------------------------------

  int soilPercentage =
    map(
      soilRaw,
      SOIL_DRY_VALUE,
      SOIL_WET_VALUE,
      0,
      100
    );


  soilPercentage =
    constrain(
      soilPercentage,
      0,
      100
    );


  // ==========================================================
  // DISPLAY
  // ==========================================================

  Serial.println();
  Serial.println();
  Serial.println("============================================================");
  Serial.println("                  CURRENT SENSOR DATA");
  Serial.println("============================================================");


  // ==========================================================
  // TABLE 1 - ADXL345
  // ==========================================================

  Serial.println();
  Serial.println("+----------------------------------------------------------+");
  Serial.println("|                       ADXL345                            |");
  Serial.println("+---------------+-------------------+----------------------+");
  Serial.println("| Axis          | Acceleration       | Unit                 |");
  Serial.println("+---------------+-------------------+----------------------+");


  Serial.print("| X             | ");
  printFloat(
    adxlXg,
    3
  );
  Serial.println(
    "              | g                    |"
  );


  Serial.print("| Y             | ");
  printFloat(
    adxlYg,
    3
  );
  Serial.println(
    "              | g                    |"
  );


  Serial.print("| Z             | ");
  printFloat(
    adxlZg,
    3
  );
  Serial.println(
    "              | g                    |"
  );


  Serial.println(
    "+---------------+-------------------+----------------------+"
  );


  // ==========================================================
  // TABLE 2 - MPU6050
  // ==========================================================

  Serial.println();
  Serial.println("+----------------------------------------------------------+");
  Serial.println("|                       MPU6050                            |");
  Serial.println("+---------------+-------------------+----------------------+");
  Serial.println("| Axis          | Acceleration       | Gyroscope (deg/s)   |");
  Serial.println("+---------------+-------------------+----------------------+");


  Serial.print("| X             | ");
  printFloat(
    mpuAccelXg,
    3
  );

  Serial.print("              | ");

  printFloat(
    gyroX,
    2
  );

  Serial.println(
    "             |"
  );


  Serial.print("| Y             | ");
  printFloat(
    mpuAccelYg,
    3
  );

  Serial.print("              | ");

  printFloat(
    gyroY,
    2
  );

  Serial.println(
    "             |"
  );


  Serial.print("| Z             | ");
  printFloat(
    mpuAccelZg,
    3
  );

  Serial.print("              | ");

  printFloat(
    gyroZ,
    2
  );

  Serial.println(
    "             |"
  );


  Serial.println(
    "+---------------+-------------------+----------------------+"
  );


  // ==========================================================
  // TABLE 3 - SOIL MOISTURE
  // ==========================================================

  Serial.println();
  Serial.println("+----------------------------------------------------------+");
  Serial.println("|                    SOIL MOISTURE                         |");
  Serial.println("+----------------------+-----------------------------------+");
  Serial.println("| Parameter            | Current Value                     |");
  Serial.println("+----------------------+-----------------------------------+");


  Serial.print("| Raw ADC Value        | ");

  Serial.print(
    soilRaw
  );

  Serial.println(
    "                              |"
  );


  Serial.print("| Moisture             | ");

  Serial.print(
    soilPercentage
  );

  Serial.println(
    " %                            |"
  );


  Serial.println(
    "+----------------------+-----------------------------------+"
  );


  // ==========================================================
  // NORMAL DISPLAY STATUS
  // ==========================================================

  Serial.println();
  Serial.println("------------------------------------------------------------");
  Serial.println("Reading completed successfully.");
  Serial.println("Next reading after 5 seconds...");
  Serial.println("------------------------------------------------------------");


  // ==========================================================
  // MINEGUARD BRIDGE DATA
  //
  // IMPORTANT:
  // This is the machine-readable line for bridge.py
  // ==========================================================

  Serial.print(
    "ADXL_X:"
  );

  Serial.print(
    adxlXg,
    3
  );


  Serial.print(
    ",ADXL_Y:"
  );

  Serial.print(
    adxlYg,
    3
  );


  Serial.print(
    ",ADXL_Z:"
  );

  Serial.print(
    adxlZg,
    3
  );


  Serial.print(
    ",MPU_AX:"
  );

  Serial.print(
    mpuAccelXg,
    3
  );


  Serial.print(
    ",MPU_AY:"
  );

  Serial.print(
    mpuAccelYg,
    3
  );


  Serial.print(
    ",MPU_AZ:"
  );

  Serial.print(
    mpuAccelZg,
    3
  );


  Serial.print(
    ",GYRO_X:"
  );

  Serial.print(
    gyroX,
    2
  );


  Serial.print(
    ",GYRO_Y:"
  );

  Serial.print(
    gyroY,
    2
  );


  Serial.print(
    ",GYRO_Z:"
  );

  Serial.print(
    gyroZ,
    2
  );


  Serial.print(
    ",SOIL_RAW:"
  );

  Serial.print(
    soilRaw
  );


  Serial.print(
    ",SOIL:"
  );

  Serial.print(
    soilPercentage
  );


  Serial.print(
    ",STATUS:NORMAL"
  );


  Serial.println(
    ",ACTION:Continue Monitoring"
  );
}


// ============================================================
// READ ADXL345
// ============================================================

void readADXL345(
  int16_t &x,
  int16_t &y,
  int16_t &z
)
{
  Wire.beginTransmission(
    ADXL345_ADDR
  );

  Wire.write(
    ADXL_DATAX0
  );

  Wire.endTransmission(
    false
  );


  Wire.requestFrom(
    ADXL345_ADDR,
    (byte)6
  );


  if (
    Wire.available() >= 6
  )
  {
    byte x0 =
      Wire.read();

    byte x1 =
      Wire.read();


    byte y0 =
      Wire.read();

    byte y1 =
      Wire.read();


    byte z0 =
      Wire.read();

    byte z1 =
      Wire.read();


    x =
      (int16_t)(
        (x1 << 8) | x0
      );

    y =
      (int16_t)(
        (y1 << 8) | y0
      );

    z =
      (int16_t)(
        (z1 << 8) | z0
      );
  }
}


// ============================================================
// READ MPU6050
// ============================================================

void readMPU6050(
  int16_t &ax,
  int16_t &ay,
  int16_t &az,
  int16_t &gx,
  int16_t &gy,
  int16_t &gz
)
{
  // ----------------------------------------------------------
  // Accelerometer
  // ----------------------------------------------------------

  Wire.beginTransmission(
    MPU6050_ADDR
  );

  Wire.write(
    MPU_ACCEL_XOUT_H
  );

  Wire.endTransmission(
    false
  );


  Wire.requestFrom(
    MPU6050_ADDR,
    (byte)6
  );


  if (
    Wire.available() >= 6
  )
  {
    byte axH =
      Wire.read();

    byte axL =
      Wire.read();


    byte ayH =
      Wire.read();

    byte ayL =
      Wire.read();


    byte azH =
      Wire.read();

    byte azL =
      Wire.read();


    ax =
      (int16_t)(
        (axH << 8) | axL
      );

    ay =
      (int16_t)(
        (ayH << 8) | ayL
      );

    az =
      (int16_t)(
        (azH << 8) | azL
      );
  }


  // ----------------------------------------------------------
  // Gyroscope
  // ----------------------------------------------------------

  Wire.beginTransmission(
    MPU6050_ADDR
  );

  Wire.write(
    MPU_GYRO_XOUT_H
  );

  Wire.endTransmission(
    false
  );


  Wire.requestFrom(
    MPU6050_ADDR,
    (byte)6
  );


  if (
    Wire.available() >= 6
  )
  {
    byte gxH =
      Wire.read();

    byte gxL =
      Wire.read();


    byte gyH =
      Wire.read();

    byte gyL =
      Wire.read();


    byte gzH =
      Wire.read();

    byte gzL =
      Wire.read();


    gx =
      (int16_t)(
        (gxH << 8) | gxL
      );

    gy =
      (int16_t)(
        (gyH << 8) | gyL
      );

    gz =
      (int16_t)(
        (gzH << 8) | gzL
      );
  }
}


// ============================================================
// WRITE REGISTER
// ============================================================

void writeRegister(
  byte deviceAddress,
  byte reg,
  byte value
)
{
  Wire.beginTransmission(
    deviceAddress
  );

  Wire.write(
    reg
  );

  Wire.write(
    value
  );

  Wire.endTransmission();
}


// ============================================================
// READ REGISTER
// ============================================================

byte readRegister(
  byte deviceAddress,
  byte reg
)
{
  Wire.beginTransmission(
    deviceAddress
  );

  Wire.write(
    reg
  );

  Wire.endTransmission(
    false
  );


  Wire.requestFrom(
    deviceAddress,
    (byte)1
  );


  if (
    Wire.available()
  )
  {
    return Wire.read();
  }


  return 0;
}


// ============================================================
// PRINT FLOAT
// ============================================================

void printFloat(
  float value,
  int decimals
)
{
  if (
    value >= 0
  )
  {
    Serial.print(" ");
  }


  Serial.print(
    value,
    decimals
  );
}