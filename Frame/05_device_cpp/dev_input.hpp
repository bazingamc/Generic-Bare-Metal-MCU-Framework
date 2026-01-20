#pragma once

/**
 * @brief Input device state enumeration
 */
typedef enum {
    InputState_Inactive = 0,//Not activated
    InputState_Active,//Activated
}InputState;

/**
 * @brief Input device parameter structure
 */
typedef struct 
{
    const char* name = "Input dev";//Device name
    GpioIndex pin = PIN_END;//Detection pin
    GpioLevel activeLevel = GPIO_LEVEL_HIGH;//Active level (when this level is detected, the input is considered active)
    uint8_t filterCount = 1;//Filter count, default is 1 (no filtering)
}InputInitParam;


/**
 * @brief Input device class
 * Used to manage GPIO input devices, supporting filtering function
 */
class Input
{
public:
    /**
     * @brief Constructor
     */
    Input();
    
    /**
     * @brief Destructor
     */
    ~Input();

    /**
     * @brief Initialize input device
     * @param param Input device parameters
     */
    void init(InputInitParam param);
    
    /**
     * @brief Check if input is in active state
     * @return true Active state, false Inactive state
     */
    bool isActive(); 
    
    /**
     * @brief Check if input is in inactive state
     * @return true Inactive state, false Active state
     */
    bool isInactive(); 
    
    /**
     * @brief Get current state
     * @return InputState Current state
     */
    InputState getState() const;
    
    /**
     * @brief Get device name
     * @return const char* Device name
     */
    const char* getName() const;


    /**********************************************************
    * Static methods
    **********************************************************/


    static void inputTask();//Called in System::run()
    static int getObjectCount();//Called in System::run()

private:
    bool isInit;//Initialization flag

    //Used to traverse all objects
	static const int MAX_OBJECTS = 50;   // Maximum object count
	static Input* objects[MAX_OBJECTS]; // Static array to store object pointers
	static int objectCount;             // Current object count

    InputInitParam initParam;//Initialization parameters
    InputState state;      // Current state
    InputState lastState;  // Previous state
    
    // Filtering related variables
    uint8_t activeCounter;   // Active state counter
    uint8_t inactiveCounter; // Inactive state counter
    bool currentReading;     // Current level reading state
};