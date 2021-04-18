
#define MEMORY_SIZE     4096
#define GFX_SIZE        64*32
#define STACK_SIZE      16
#define REGISTER_SIZE   16
#define KEYPAD_SIZE     16

#define X_VAL   ((opcode & 0x0F00) >> 8)
#define Y_VAL   ((opcode & 0x00F0) >> 4)

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

    private:
        ///< Opcodes in the chip8 are 2 bytes long
        unsigned short opcode;
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


        unsigned short fetchOpcode();
        void clearDisp();
};