
#define MEMORY_SIZE     4096
#define GFX_SIZE        64*32
#define STACK_SIZE      16
#define REGISTER_SIZE   16
#define KEYPAD_SIZE     16

#define NUM_OPCODES     35

#define X_VAL   ((opcode & 0x0F00) >> 8)
#define Y_VAL   ((opcode & 0x00F0) >> 4)

// #define CALL_MEMBER_FN(object, ptrToMember) ((object).*(ptrToMember))
#define CALL_MEMBER_FN(ptrToMember) (ptrToMember)

/**
 * Definition of opcode fuinction pointers
 */
// void (*opcode_function)();

// void (*opcode_function_table[NUM_OPCODES])(void) = {
//     {opcode_ONNN}, {opcode_00E0},
//     {opcode_00EE}, {opcode_1NNN},
//     {opcode_2NNN}, {opcode_3XNN},
//     {opcode_4XNN}, {opcode_5XY0},
//     {opcode_6XNN}, {opcode_7XNN},
//     {opcode_8XY0}, {opcode_8XY1},
//     {opcode_8XY2}, {opcode_8XY3},
//     {opcode_8XY4}, {opcode_8XY5},
//     {opcode_8XY6}, {opcode_8XY7},
//     {opcode_8XYE}, {opcode_9XY0},
//     {opcode_ANNN}, {opcode_BNNN},
//     {opcode_CXNN}, {opcode_DXYN},
//     {opcode_EX9E}, {opcode_EXA1},
//     {opcode_FX07}, {opcode_FX0A},
//     {opcode_FX15}, {opcode_FX18},
//     {opcode_FX1E}, {opcode_FX29},
//     {opcode_FX33}, {opcode_FX55},
//     {opcode_FX65}
// };

/**
 * Array of 
 * 
 */

class chip8
{
    public:
        bool drawFlag = false;
        ///< Chip 8 keypad
        unsigned char key[KEYPAD_SIZE];
        ///< Chip 8 graphics
        unsigned char gfx[GFX_SIZE];
        
        void initialize();
        void emulateCycle();
        bool loadGame(const char * romName);

        typedef void (chip8::*OpcodeMemFun)();

        OpcodeMemFun opcodeArray[NUM_OPCODES] = 
        {&chip8::opcode_ONNN, &chip8::opcode_00E0,
        &chip8::opcode_00EE, &chip8::opcode_1NNN,
        &chip8::opcode_2NNN, &chip8::opcode_3XNN,
        &chip8::opcode_4XNN, &chip8::opcode_5XY0,
        &chip8::opcode_6XNN, &chip8::opcode_7XNN,
        &chip8::opcode_8XY0, &chip8::opcode_8XY1,
        &chip8::opcode_8XY2, &chip8::opcode_8XY3,
        &chip8::opcode_8XY4, &chip8::opcode_8XY5,
        &chip8::opcode_8XY6, &chip8::opcode_8XY7,
        &chip8::opcode_8XYE, &chip8::opcode_9XY0,
        &chip8::opcode_ANNN, &chip8::opcode_BNNN,
        &chip8::opcode_CXNN, &chip8::opcode_DXYN,
        &chip8::opcode_EX9E, &chip8::opcode_EXA1,
        &chip8::opcode_FX07, &chip8::opcode_FX0A,
        &chip8::opcode_FX15, &chip8::opcode_FX18,
        &chip8::opcode_FX1E, &chip8::opcode_FX29,
        &chip8::opcode_FX33, &chip8::opcode_FX55,
        &chip8::opcode_FX65
        };
        // void (chip8::*opcode_function_table[NUM_OPCODES])() = 
        // {&chip8::opcode_ONNN, &chip8::opcode_00E0,
        // &chip8::opcode_00EE, &chip8::opcode_1NNN,
        // &chip8::opcode_2NNN, &chip8::opcode_3XNN,
        // &chip8::opcode_4XNN, &chip8::opcode_5XY0,
        // &chip8::opcode_6XNN, &chip8::opcode_7XNN,
        // &chip8::opcode_8XY0, &chip8::opcode_8XY1,
        // &chip8::opcode_8XY2, &chip8::opcode_8XY3,
        // &chip8::opcode_8XY4, &chip8::opcode_8XY5,
        // &chip8::opcode_8XY6, &chip8::opcode_8XY7,
        // &chip8::opcode_8XYE, &chip8::opcode_9XY0,
        // &chip8::opcode_ANNN, &chip8::opcode_BNNN,
        // &chip8::opcode_CXNN, &chip8::opcode_DXYN,
        // &chip8::opcode_EX9E, &chip8::opcode_EXA1,
        // &chip8::opcode_FX07, &chip8::opcode_FX0A,
        // &chip8::opcode_FX15, &chip8::opcode_FX18,
        // &chip8::opcode_FX1E, &chip8::opcode_FX29,
        // &chip8::opcode_FX33, &chip8::opcode_FX55,
        // &chip8::opcode_FX65
        // };

    private:
        ///< Opcodes in the chip8 are 2 bytes long
        unsigned short opcode;
        unsigned short mappedOpcode;
        ///< The Chip 8 has 4k in memory
        unsigned char memory[MEMORY_SIZE];
        ///< The Chip 8 has 15 general purpose registers and a 16th register used for a carry flag
        unsigned char V[REGISTER_SIZE];
        ///< Index register and program counter
        unsigned short I;
        unsigned short pc;
        ///< Chip 8 timers
        unsigned char delay_timer;
        unsigned char sound_timer;
        ///< Chip 8 stack and stack pointer
        unsigned short stack[STACK_SIZE];
        unsigned short sp;




        // void (chip8::*opcode_function_table[2])() = {&chip8::opcode_ONNN, &chip8::opcode_00E0};

        unsigned short fetchOpcode();
        void clearDisp();

        ///< Opcode Helper functions
        int opcodeMap(unsigned short opcode);

        ///< Opcode functions
        void opcode_ONNN();
        void opcode_00E0();
        void opcode_00EE();
        void opcode_1NNN();
        void opcode_2NNN();
        void opcode_3XNN();
        void opcode_4XNN();
        void opcode_5XY0();
        void opcode_6XNN();
        void opcode_7XNN();
        void opcode_8XY0();
        void opcode_8XY1();
        void opcode_8XY2();
        void opcode_8XY3();
        void opcode_8XY4();
        void opcode_8XY5();
        void opcode_8XY6();
        void opcode_8XY7();
        void opcode_8XYE();
        void opcode_9XY0();
        void opcode_ANNN();
        void opcode_BNNN();
        void opcode_CXNN();
        void opcode_DXYN();
        void opcode_EX9E();
        void opcode_EXA1();
        void opcode_FX07();
        void opcode_FX0A();
        void opcode_FX15();
        void opcode_FX18();
        void opcode_FX1E();
        void opcode_FX29();
        void opcode_FX33();
        void opcode_FX55();
        void opcode_FX65();
};