#pragma once

/**
 * @brief Output device state enumeration
 */
typedef enum {
    OutState_Close = 0,//Closed
    OutState_Open,//Opened
}OutState;

/**
 * @brief Output device parameter structure
 */
typedef struct 
{
    const char* name = "Output dev";//Device name
    GpioIndex pin = PIN_END;//Control pin
    GpioLevel validLevel = GPIO_LEVEL_LOW;//Valid level (the level output by the pin when the device is opened)
}OutputInitParam;


/**
 * @brief Output device class
 * Used to manage GPIO output devices
 */
class Output
{
public:
    /**
     * @brief Constructor
     */
    Output();
    
    /**
     * @brief Destructor
     */
    ~Output();

    /**
     * @brief Initialize output device
     * @param param Output device parameters
     */
    void init(OutputInitParam param);
    
    /**
     * @brief Open output device
     */
    void open(); 
    
    /**
     * @brief Close output device
     */
    void close(); 
    
    /**
     * @brief Start pulse output
     * @param cycle Period, default 1000ms
     * @param duty Duty cycle, default 50%
     */
    void pulseOutputStart(u32 cycle = 1000 , float duty = 50); 
    
    /**
     * @brief Stop pulse output
     */
    void pulseOutputStop(); 
    
    /**
     * @brief Get device name
     * @return const char* Device name
     */
    const char* getName() const;
    
    /**
     * @brief Get current state
     * @return OutState Current state
     */
    OutState getState() const;
    
    /**
     * @brief Get current duty cycle
     * @return float Duty cycle value
     */
    float getDuty() const;


    /**********************************************************
    * Static methods
    **********************************************************/

    static void outputTask();//Called in System::run()
    static int getObjectCount();//Called in System::run()

private:
    bool isInit;//Initialization flag

    //Used to traverse all objects
	static const int MAX_OBJECTS = 50;   // Maximum object count
	static Output* objects[MAX_OBJECTS]; // Static array to store object pointers
	static int objectCount;               // Current object count

    OutputInitParam initParam;//Initialization parameters
    float duty;//Duty cycle 0 - 100
    u32 cycle;//Period in ms
    OutState state;

};