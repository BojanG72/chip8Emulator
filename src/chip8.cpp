#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "chip8.h"

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
    // printf("Executing: 0x%X\n", opcode);
    ///< Decode Opcode
    switch(opcode & 0xF000)
    {
        case 0x0000:
        {
            switch(opcode & 0x000F)
            {
                case 0x0000: ///< opcode 0x00E0
                    ///< clear screen
                    clearDisp();
                    pc += 2;
                break;
                //////////////////////////////////////////////////////
                case 0x000E: ///< opcode 0x00EE
                    --sp;
                    pc = stack[sp];
                    pc += 2;
                break;
                //////////////////////////////////////////////////////
                default:
                    printf("unkown opcode 0x%X\n", opcode);

            }
        }
        break;
        //////////////////////////////////////////////////////////////
        case 0x1000: ///< opcode 0x1NNN
        {
            pc = opcode & 0x0FFF; ///< Jump to address NNN    
        }
        break;
        //////////////////////////////////////////////////////////////
        case 0x2000: ///< opcode 0x2NNN
        {
            stack[sp] = pc;         ///< Save the PC in the stack
            ++sp;                   ///< Increment the stack pointer
            pc = opcode & 0x0FFF;   ///< Jump to NNN
        }
        break;
        //////////////////////////////////////////////////////////////
        case 0x3000: ///< opcode 0x3XNN
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
        break;
        //////////////////////////////////////////////////////////////
        case 0x4000: ///< opcode 0x4XNN
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
        break;
        //////////////////////////////////////////////////////////////
        case 0x5000: ///< opcode 0x5XY0
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
        break;
        //////////////////////////////////////////////////////////////
        case 0x6000: ///< opcode 0x6XNN
        {
            ///< Set Vx to NN
            V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
            pc += 2;
        }
        break;
        //////////////////////////////////////////////////////////////
        case 0x7000: ///< opcode 0x7XNN
        {
            ///< add NN to Vx
            V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
            pc += 2; 
        }
        break;
        //////////////////////////////////////////////////////////////
        case 0x8000:
            switch (opcode & 0x000F)
            {
                case 0x0000: ///< opcode 0x8XY0
                {
                    ///< Vx = Vy
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                }
                break;
                case 0x0001: ///< opcode 0x8XY1
                {
                    ///< Vx = Vx|Vy
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                }
                break;
                case 0x0002: ///< opcode 0x8XY2
                {
                    ///< Vx = Vx&Vy
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                }
                break;
                case 0x0003: ///< opcode 0x8XY3
                {
                    ///< Vx = Vx^Vy
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4];
                    pc += 2;

                }
                break;
                case 0x0004: ///< opcode 0x8XY4
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
                break;
                case 0x0005: ///< opcode 0x8XY5
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
                break;
                case 0x0006: ///< opcode 0x8XY6
                {
                    ///< Vx >>= 1
                    V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
                    V[(opcode & 0x0F00) >> 8] >>= 1;
                    pc += 2;

                }
                break;
                case 0x0007: ///< opcode 0x8XY7
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
                break;
                case 0x000E: ///< opcode 0x8XYE
                {
                    ///< Vx <<= 1
                    V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
                    V[(opcode & 0x0F00) >> 8] <<= 1;
                    pc += 2;

                }
                break;

                default:
                    printf("Unknown opcode [0x8000]: 0x%X\n", opcode);
            }

        break;
        //////////////////////////////////////////////////////////////
        case 0x9000: ///< opcode 9XY0
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
        break;
        //////////////////////////////////////////////////////////////
        case 0xA000: ///< opcode ANNN
        {
            ///< Execute Opcode
            I = opcode & 0x0FFF;
            pc += 2;
        }
        break;
        //////////////////////////////////////////////////////////////
        case 0xB000: ///< opcode 0xBNNN
        {
            ///< Jump tp V[0] + NNN
            pc = V[0] + (opcode & 0x0FFF);
        }
        break;
        //////////////////////////////////////////////////////////////
        case 0xC000: ///< opcode 0xCXNN
        {
            V[X_VAL] = (rand() % 0xFF) & (opcode & 0x00FF);
            pc += 2;
        }
        break;
        //////////////////////////////////////////////////////////////
        case 0xD000: ///< opcode DXYN (Graphics)
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
        break;
        //////////////////////////////////////////////////////////////
        case 0xE000:
        {
            switch (opcode & 0x00FF)
            {
                case 0x009E: ///< opcode 0xEX9E
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
                break;
                //////////////////////////////////////////////////////////////
                case 0x00A1: ///< opcode 0xEXA1
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
                break;
                //////////////////////////////////////////////////////////////
                default:
                    printf("Unknown opcode [0xE000]: 0x%X\n", opcode);
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
                    V[X_VAL] = delay_timer;
                    pc += 2;
                }
                break;
                case 0x000A: ///< opcode 0xFX0A (key operation)
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
                case 0x0015: ///< opcode 0xFX15
                {
                    delay_timer = V[X_VAL];
                    pc += 2;
                }
                break;
                case 0x0018: ///< opcode 0xFX18
                {
                    sound_timer = V[X_VAL];
                    pc += 2;
                }
                break;
                case 0x001E: ///< opcode 0xFX1E
                {
                    I += V[X_VAL];
                    pc += 2;
                }
                break;
                case 0x0029: ///< opcode 0xFX29
                {
                    I = V[X_VAL] * 0x5;
                    pc += 2;
                }
                break;
                case 0x0033: ///< opcode 0xFX33
                {
                    memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
                    memory[I+1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
                    memory[I+2] = (V[(opcode & 0x0F00) >> 8]  % 100) % 10;
                    pc += 2;
                }
                break;
                case 0x0055: ///< opcode 0xFX55
                {
                    for(int i = 0; i <= X_VAL; i++)
                    {
                        memory[I + i] = V[i];
                    }
                    I += X_VAL + 1;
                    pc += 2;
                }
                break;
                case 0x0065: ///< opcode 0xFX65
                {
                    for(int i = 0; i <= X_VAL; i++)
                    {
                        V[i] = memory[I + i];
                    }
                    I += X_VAL + 1;
                    pc += 2;

                }
                break;

                default:
                    printf("Unknown opcode [0xF000]: 0x%X\n", opcode);
            }
        break;
        /////////////////////////////////////////////////////////////
        default:
            printf("Unkown Opcode: 0x%X\n", opcode);
    }

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
