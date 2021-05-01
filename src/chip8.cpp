#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "chip8.h"
#include <functional>
#include <iostream>

typedef enum {
    OPCODE_ONNN,
    OPCODE_00E0,
    OPCODE_00EE,
    OPCODE_1NNN,
    OPCODE_2NNN,
    OPCODE_3XNN,
    OPCODE_4XNN,
    OPCODE_5XY0,
    OPCODE_6XNN,
    OPCODE_7XNN,
    OPCODE_8XY0,
    OPCODE_8XY1,
    OPCODE_8XY2,
    OPCODE_8XY3,
    OPCODE_8XY4,
    OPCODE_8XY5,
    OPCODE_8XY6,
    OPCODE_8XY7,
    OPCODE_8XYE,
    OPCODE_9XY0,
    OPCODE_ANNN,
    OPCODE_BNNN,
    OPCODE_CXNN,
    OPCODE_DXYN,
    OPCODE_EX9E,
    OPCODE_EXA1,
    OPCODE_FX07,
    OPCODE_FX0A,
    OPCODE_FX15,
    OPCODE_FX18,
    OPCODE_FX1E,
    OPCODE_FX29,
    OPCODE_FX33,
    OPCODE_FX55,
    OPCODE_FX65
} OPCODE_t;

const unsigned char chip8_fontset[80] =
{ 
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


void chip8::initialize()
{
    ///< The program counter starts a position 0x200
    pc = 0x200;
    ///< Reset the current opcode, index register, and stack pointer
    opcode = 0;
    I = 0;
    sp = 0;
    ///< Clear display, stack, registers, and memory
    memset(gfx, 0, GFX_SIZE);
    memset(stack, 0, sizeof(unsigned short)*STACK_SIZE);
    memset(V, 0, REGISTER_SIZE);
    memset(memory, 0, MEMORY_SIZE);

    ///< Load the font set
    for(int i = 0; i<80; ++i)
    {
        memory[i] = chip8_fontset[i];
    }

    ///< Reset timers
    delay_timer = 60;
    sound_timer = 60;

    srand (time(NULL));
}

void chip8::clearDisp()
{
    memset(gfx, 0, GFX_SIZE);
}

bool chip8::loadGame(const char *romName)
{
    initialize();
    printf("Loading: %s\n", romName);
    char buffer[4096];
    memset(buffer, 0, 4096);


    FILE *fptr;
    fptr = fopen(romName, "rb");

    if(fptr == NULL)
    {
        fputs("File Error", stderr);
        return false;
    }

    ///< Check the file size
    fseek(fptr, 0, SEEK_END);
    long fileSize = ftell(fptr);
    printf("Filesize: %d\n", (int)fileSize);

    fseek(fptr, 0, SEEK_SET);
    size_t bytesRead = fread(buffer, 1, fileSize, fptr);
    if (bytesRead != (size_t)fileSize)
    {
        fputs("reading error", stderr);
        return false;
    }

    ///< copy buffer to memory
    for(int i = 0; i<fileSize; i++)
    {
        memory[0x200 + i] = buffer[i];
    }

    fclose(fptr);
    return true;
}

void chip8::emulateCycle()
{
    ///< Fetch Opcode
    opcode = fetchOpcode();

    ///< Decode Opcode
    opcodeMap(opcode);
    
    ///< Update Timers
    if(delay_timer > 0)
    {
        delay_timer--;
    }
    if(sound_timer > 0){
        if(sound_timer == 1)
        {
            printf("BEEP!!\n");
        }
        --sound_timer;
    }
}

unsigned short chip8::fetchOpcode()
{
    return ((memory[pc] << 8) | (memory[pc + 1]));
}

int chip8::opcodeMap(unsigned short opcode)
{
        switch(opcode & 0xF000)
    {
        case 0x0000:
        {
            switch(opcode & 0x000F)
            {
                case 0x0000: ///< opcode 0x00E0
                    opcode_00E0();
                    return OPCODE_00E0;
                break;
                //////////////////////////////////////////////////////
                case 0x000E: ///< opcode 0x00EE
                    opcode_00EE();
                    return OPCODE_00EE;
                break;
                //////////////////////////////////////////////////////
                default:
                    printf("unkown opcode 0x%X\n", opcode);
                    return -1;

            }
        }
        break;
        //////////////////////////////////////////////////////////////
        case 0x1000: ///< opcode 0x1NNN
        {
            opcode_1NNN();
            return OPCODE_1NNN;  
        }
        break;
        //////////////////////////////////////////////////////////////
        case 0x2000: ///< opcode 0x2NNN
        {
            opcode_2NNN();
            return OPCODE_2NNN;
        }
        break;
        //////////////////////////////////////////////////////////////
        case 0x3000: ///< opcode 0x3XNN
        {
            opcode_3XNN();
            return OPCODE_3XNN;
        }
        break;
        //////////////////////////////////////////////////////////////
        case 0x4000: ///< opcode 0x4XNN
        {
            opcode_4XNN();
            return OPCODE_4XNN;
        }
        break;
        //////////////////////////////////////////////////////////////
        case 0x5000: ///< opcode 0x5XY0
        {
            opcode_5XY0();
            return OPCODE_5XY0;
        }
        break;
        //////////////////////////////////////////////////////////////
        case 0x6000: ///< opcode 0x6XNN
        {
            opcode_6XNN();
            return OPCODE_6XNN;
        }
        break;
        //////////////////////////////////////////////////////////////
        case 0x7000: ///< opcode 0x7XNN
        {
            opcode_7XNN();
            return OPCODE_7XNN;
        }
        break;
        //////////////////////////////////////////////////////////////
        case 0x8000:
            switch (opcode & 0x000F)
            {
                case 0x0000: ///< opcode 0x8XY0
                {
                    opcode_8XY0();
                    return OPCODE_8XY0;
                }
                break;
                case 0x0001: ///< opcode 0x8XY1
                {
                    opcode_8XY1();
                    return OPCODE_8XY1;
                }
                break;
                case 0x0002: ///< opcode 0x8XY2
                {
                    opcode_8XY2();
                    return OPCODE_8XY2;
                }
                break;
                case 0x0003: ///< opcode 0x8XY3
                {
                    opcode_8XY3();
                    return OPCODE_8XY3;
                }
                break;
                case 0x0004: ///< opcode 0x8XY4
                {
                    opcode_8XY4();
                    return OPCODE_8XY4;
                }
                break;
                case 0x0005: ///< opcode 0x8XY5
                {
                    opcode_8XY5();
                    return OPCODE_8XY5;
                }
                break;
                case 0x0006: ///< opcode 0x8XY6
                {
                    opcode_8XY6();
                    return OPCODE_8XY6;
                }
                break;
                case 0x0007: ///< opcode 0x8XY7
                {
                    opcode_8XY7();
                    return OPCODE_8XY7;
                }
                break;
                case 0x000E: ///< opcode 0x8XYE
                {
                    opcode_8XYE();
                    return OPCODE_8XYE;
                }
                break;

                default:
                    printf("Unknown opcode [0x8000]: 0x%X\n", opcode);
                    return -1;
            }

        break;
        //////////////////////////////////////////////////////////////
        case 0x9000: ///< opcode 9XY0
        {
            opcode_9XY0();
            return OPCODE_9XY0;
        }
        break;
        //////////////////////////////////////////////////////////////
        case 0xA000: ///< opcode ANNN
        {
            opcode_ANNN();
            return OPCODE_ANNN;
        }
        break;
        //////////////////////////////////////////////////////////////
        case 0xB000: ///< opcode 0xBNNN
        {
            opcode_BNNN();
            return OPCODE_BNNN;
        }
        break;
        //////////////////////////////////////////////////////////////
        case 0xC000: ///< opcode 0xCXNN
        {
            opcode_CXNN();
            return OPCODE_CXNN;
        }
        break;
        //////////////////////////////////////////////////////////////
        case 0xD000: ///< opcode DXYN (Graphics)
        {
            opcode_DXYN();
            return OPCODE_DXYN;
        }
        break;
        //////////////////////////////////////////////////////////////
        case 0xE000:
        {
            switch (opcode & 0x00FF)
            {
                case 0x009E: ///< opcode 0xEX9E
                {
                    opcode_EX9E();
                    return OPCODE_EX9E;
                }
                break;
                //////////////////////////////////////////////////////////////
                case 0x00A1: ///< opcode 0xEXA1
                {
                    opcode_EXA1();
                    return OPCODE_EXA1;
                }
                break;
                //////////////////////////////////////////////////////////////
                default:
                    printf("Unknown opcode [0xE000]: 0x%X\n", opcode);
                    return -1;
            }
        }
        break;
        //////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////
        case 0xF000:
            switch (opcode & 0x00FF)
            {
                case 0x0007: ///< opcode 0xFX07
                {
                    opcode_FX07();
                    return OPCODE_FX07;
                }
                break;
                case 0x000A: ///< opcode 0xFX0A (key operation)
                {
                    opcode_FX0A();
                    return OPCODE_FX0A;
                }
                case 0x0015: ///< opcode 0xFX15
                {
                    opcode_FX15();
                    return OPCODE_FX15;
                }
                break;
                case 0x0018: ///< opcode 0xFX18
                {
                    opcode_FX18();
                    return OPCODE_FX18;
                }
                break;
                case 0x001E: ///< opcode 0xFX1E
                {
                    opcode_FX1E();
                    return OPCODE_FX1E;
                }
                break;
                case 0x0029: ///< opcode 0xFX29
                {
                    opcode_FX29();
                    return OPCODE_FX29;
                }
                break;
                case 0x0033: ///< opcode 0xFX33
                {
                    opcode_FX33();
                    return OPCODE_FX33;
                }
                break;
                case 0x0055: ///< opcode 0xFX55
                {
                    opcode_FX55();
                    return OPCODE_FX55;
                }
                break;
                case 0x0065: ///< opcode 0xFX65
                {
                    opcode_FX65();
                    return OPCODE_FX65;
                }
                break;

                default:
                    printf("Unknown opcode [0xF000]: 0x%X\n", opcode);
            }
        break;
        /////////////////////////////////////////////////////////////
        default:
            printf("Unkown Opcode: 0x%X\n", opcode);
            return -1;
    }

    return -1;
}



////////////////////////////////////////////////////////////////////
///< Opcode functions
////////////////////////////////////////////////////////////////////
void chip8::opcode_ONNN()
{
    ///< Not Implemented
}

void chip8::opcode_00E0()
{
    ///< clear screen
    clearDisp();
    pc += 2;
}

void chip8::opcode_00EE()
{
    --sp;
    pc = stack[sp];
    pc += 2;
}

void chip8::opcode_1NNN()
{
    pc = opcode & 0x0FFF; ///< Jump to address NNN  
}

void chip8::opcode_2NNN()
{
    stack[sp] = pc;         ///< Save the PC in the stack
    ++sp;                   ///< Increment the stack pointer
    pc = opcode & 0x0FFF;   ///< Jump to NNN
}
void chip8::opcode_3XNN()
{
    ///< if Vx = NN, skip next instruction
    if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
    {
        pc += 4;
    }
    else
    {
        pc += 2;
    }
}
void chip8::opcode_4XNN()
{
    ///< if Vx != NN, skip next instruction
    if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
    {
        pc += 4;
    }
    else
    {
        pc += 2;
    }
}
void chip8::opcode_5XY0()
{
    ///< if Vx = Vy, skip next instruction
    if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
    {
        pc += 4;
    }
    else
    {
        pc += 2;
    }
}
void chip8::opcode_6XNN()
{
    ///< Set Vx to NN
    V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
    pc += 2;
}
void chip8::opcode_7XNN()
{
    ///< add NN to Vx
    V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
    pc += 2; 
}

void chip8::opcode_8XY0()
{
    ///< Vx = Vy
    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
    pc += 2;
}
void chip8::opcode_8XY1()
{
    ///< Vx = Vx|Vy
    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4];
    pc += 2;
}
void chip8::opcode_8XY2()
{
    ///< Vx = Vx&Vy
    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4];
    pc += 2;
}
void chip8::opcode_8XY3()
{
    ///< Vx = Vx^Vy
    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4];
    pc += 2;
}
void chip8::opcode_8XY4()
{
    if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
    {
        V[0xF] = 1; ///< V[Y] + V[X] > 255, carry
    }
    else
    {
        V[0xF] = 0;
    }
    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
    pc += 2;
}
void chip8::opcode_8XY5()
{
    ///< Vx = Vx-Vy
    if(V[(opcode & 0x00F0) >> 4] > (V[(opcode & 0x0F00) >> 8]))
    {
        V[0xF] = 0; ///< Vy > Vx, borrow
    }
    else
    {
        V[0xF] = 1;
    }
    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
    pc += 2;

}
void chip8::opcode_8XY6()
{
    ///< Vx >>= 1
    V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
    V[(opcode & 0x0F00) >> 8] >>= 1;
    pc += 2;
}
void chip8::opcode_8XY7()
{
    ///< Vx = Vy - Vx
    if(V[(opcode & 0x0F00) >> 8] > (V[(opcode & 0x00F0) >> 4]))
    {
        V[0xF] = 0; ///< Vx > Vy, borrow
    }
    else
    {
        V[0xF] = 1;
    }
    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
    pc += 2;
}
void chip8::opcode_8XYE()
{
    ///< Vx <<= 1
    V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
    V[(opcode & 0x0F00) >> 8] <<= 1;
    pc += 2;
}
void chip8::opcode_9XY0()
{
    ///< Skip instruction if Vx != Vy
    if(V[X_VAL] != V[Y_VAL])
    {
        pc += 4;
    }
    else
    {
        pc += 2;
    }
}
void chip8::opcode_ANNN()
{
    ///< Execute Opcode
    I = opcode & 0x0FFF;
    pc += 2;
}
void chip8::opcode_BNNN()
{
    ///< Jump tp V[0] + NNN
    pc = V[0] + (opcode & 0x0FFF);
}
void chip8::opcode_CXNN()
{
    V[X_VAL] = (rand() % 0xFF) & (opcode & 0x00FF);
    pc += 2;
}
void chip8::opcode_DXYN()
{
    unsigned short x = V[(opcode & 0x0F00) >> 8];
    unsigned short y = V[(opcode & 0x00F0) >> 4];
    unsigned short height = (opcode & 0x000F);
    unsigned short pixel;

    V[0xF] = 0;
    for (int yline = 0; yline < height; yline++)
    {
        pixel = memory[I + yline];
        for (int xline = 0; xline < 8; xline++)
        {
            if((pixel & (0x80 >> xline)) != 0)
            {
                if(gfx[(x + xline + ((y + yline)*64))] == 1)
                {
                    V[0xF] = 1;
                }
                gfx[(x + xline + ((y + yline)*64))] ^= 1;
            }
        }
    }

    drawFlag = true;
    pc += 2;
}
void chip8::opcode_EX9E()
{
    if(key[V[(opcode & 0x0F00) >> 8]] != 0)
    {
        pc += 4;
    }
    else
    { 
        pc += 2;
    }
}
void chip8::opcode_EXA1()
{
    if(key[V[(opcode & 0x0F00) >> 8]] == 0)
    {
        pc += 4;
    }
    else
    { 
        pc += 2;
    }
}
void chip8::opcode_FX07()
{
    V[X_VAL] = delay_timer;
    pc += 2;
}
void chip8::opcode_FX0A()
{
    bool keyPress = false;

    for (int i = 0; i < 16; i++)
    {   
        ///< Check if any key have been pressed
        if(key[i] != 0)
        {
            V[(opcode & 0x0F00) >> 8] = i;
            keyPress = true;
        }
    }

    if(!keyPress)
    {
        ///< Key press was not detected, keep waiting
        return;
    }
    
    
    pc += 2;    ///< incrememnt PC to next instruction
}
void chip8::opcode_FX15()
{
    delay_timer = V[X_VAL];
    pc += 2;
}
void chip8::opcode_FX18()
{
    sound_timer = V[X_VAL];
    pc += 2;
}
void chip8::opcode_FX1E()
{
    I += V[X_VAL];
    pc += 2;
}
void chip8::opcode_FX29()
{
    I = V[X_VAL] * 0x5;
    pc += 2;
}
void chip8::opcode_FX33()
{
    memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
    memory[I+1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
    memory[I+2] = (V[(opcode & 0x0F00) >> 8]  % 100) % 10;
    pc += 2;
}
void chip8::opcode_FX55()
{
    for(int i = 0; i <= X_VAL; i++)
    {
        memory[I + i] = V[i];
    }
    I += X_VAL + 1;
    pc += 2;
}
void chip8::opcode_FX65()
{
    for(int i = 0; i <= X_VAL; i++)
    {
        V[i] = memory[I + i];
    }
    I += X_VAL + 1;
    pc += 2;
}
