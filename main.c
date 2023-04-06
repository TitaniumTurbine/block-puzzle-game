#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdint.h>

int ch;
int screenWidth = 22;
int screenHeight = 20;
int screenSize = 22 * 20;
int xPos[4];
int yPos[4];
char screen[22 * 20 + 1];
int64_t moveTime;
int blockID;
int rotID;

int64_t currentTimeMillis() {
    struct timeval time;
    gettimeofday(&time, NULL);
    int64_t s1 = (int64_t)(time.tv_sec) * 1000;
    int64_t s2 = (time.tv_usec / 1000);
    return s1 + s2;
}

void clearBlock(){
    for(int i = 0; i < 4; i++){
        screen[(screenWidth * yPos[i] + xPos[i])] = ' ';
        screen[(screenWidth * yPos[i] + xPos[i])+1] = ' ';
    }
    
}

void drawBlock(){
    for(int i = 0; i < 4; i++){
        screen[(screenWidth * yPos[i] + xPos[i])] = '[';
        screen[(screenWidth * yPos[i] + xPos[i])+1] = ']';
    }
    printf("\033[H%s", screen); // return cursor to start and print screen
}

int moveDown(){
    int selfCollideFlag;
    // check all block tiles can move
    for(int x = 0; x < 4; x++){
        selfCollideFlag = 0;
        if(screen[(screenWidth * (yPos[x] + 1) + xPos[x])] != ' '){
            // check that not colliding with another tile from the block
            for(int i = 0; i < 4; i++){
                if(yPos[x] + 1 == yPos[i] && xPos[x] == xPos[i]) selfCollideFlag = 1;
            }
            if(selfCollideFlag == 0) return 0;   // if none can move then exit and return 0
        }
    }

    // move all tiles down
    clearBlock();
    for(int y = 0; y < 4; y++){
        yPos[y] += 1;
    }
    drawBlock();
    moveTime = currentTimeMillis();

    return 1;
}

void newBlock(int blockType){
    // square
    if(blockType == 0){
        yPos[0] = 1;
        yPos[1] = 1;
        yPos[2] = 2;
        yPos[3] = 2;

        xPos[0] = (screenWidth / 2) - 1;
        xPos[1] = (screenWidth / 2) + 1;
        xPos[2] = (screenWidth / 2) - 1;
        xPos[3] = (screenWidth / 2) + 1;
    }

    // line
    if(blockType == 1){
        yPos[0] = 1;
        yPos[1] = 2;
        yPos[2] = 3;
        yPos[3] = 4;

        xPos[0] = (screenWidth / 2) - 1;
        xPos[1] = (screenWidth / 2) - 1;
        xPos[2] = (screenWidth / 2) - 1;
        xPos[3] = (screenWidth / 2) - 1;
    }

    // L
    if(blockType == 2){
        yPos[0] = 1;
        yPos[1] = 2;
        yPos[2] = 3;
        yPos[3] = 3;

        xPos[0] = (screenWidth / 2) - 1;
        xPos[1] = (screenWidth / 2) - 1;
        xPos[2] = (screenWidth / 2) - 1;
        xPos[3] = (screenWidth / 2) + 1;
    }

    // reverse L
    if(blockType == 3){
        yPos[0] = 1;
        yPos[1] = 2;
        yPos[2] = 3;
        yPos[3] = 3;

        xPos[0] = (screenWidth / 2) + 1;
        xPos[1] = (screenWidth / 2) + 1;
        xPos[2] = (screenWidth / 2) + 1;
        xPos[3] = (screenWidth / 2) - 1;
    }

    // squiggly
    if(blockType == 4){
        yPos[0] = 2;
        yPos[1] = 2;
        yPos[2] = 1;
        yPos[3] = 1;

        xPos[0] = (screenWidth / 2) - 1;
        xPos[1] = (screenWidth / 2) + 1;
        xPos[2] = (screenWidth / 2) + 1;
        xPos[3] = (screenWidth / 2) + 3;
    }

    // reverse squiggly
    if(blockType == 5){
        yPos[0] = 1;
        yPos[1] = 2;
        yPos[2] = 1;
        yPos[3] = 2;

        xPos[0] = (screenWidth / 2) - 1;
        xPos[1] = (screenWidth / 2) + 1;
        xPos[2] = (screenWidth / 2) + 1;
        xPos[3] = (screenWidth / 2) + 3;
    }

    // T
    if(blockType == 6){
        yPos[0] = 1;
        yPos[1] = 2;
        yPos[2] = 2;
        yPos[3] = 2;

        xPos[0] = (screenWidth / 2) - 1;
        xPos[1] = (screenWidth / 2) - 3;
        xPos[2] = (screenWidth / 2) - 1;
        xPos[3] = (screenWidth / 2) + 1;
    }
    
    drawBlock();
}

void rotate(){
    clearBlock();
    int pivotX = xPos[1];
    int pivotY = yPos[1];
    int oldX[4];
    int oldY[4];
    for(int i = 0; i < 4; i++){
        oldX[i] = xPos[i];
        oldY[i] = yPos[i];
        yPos[i] = (oldX[i] - pivotX) / 2 + pivotY;
        xPos[i] = 2 * (pivotY - oldY[i]) + pivotX;
    }

    // check intersections with newlines and correct
    for(int i = 0; i < 4; i++){
        if(screen[(screenWidth * yPos[i] + xPos[i])] == '\n' || screen[(screenWidth * yPos[i] + xPos[i] + 1)] == '\n'){
            // right side
            if(xPos[i] > screenWidth / 2){
                for(int j = 0; j < 4; j++){
                    xPos[j] -= 2;
                }
            }
            
            // left side
            else{
                for(int j = 0; j < 4; j++){
                    xPos[j] += 2;
                }
            }
        }
    }

    drawBlock();
}

int main(){
    // \033[A - go up
    // \33[2K\r - delete line
    // \e[?25l - hide cursor
    // \e[?25h - show cursor

    moveTime = currentTimeMillis();

    srand(time(NULL));

    for (int i = 0; i < screenSize; i++){
        screen[i] = ' ';
    }

    for(int i = screenSize; i > screenSize - screenWidth; i--){
        screen[i] = '=';
    }
    
    for(int i = 1; i < screenHeight; i++){
        screen[screenWidth * i] = '\n';
    }

    printf("\e[?25l"); // hide cursor
    printf("\033[2J"); // clear screen
    newBlock(rand() % (6 + 1));

    int lineCount = 0;
    int moveFlag;
    int play = 1;
    int lines = 0;
    while(play){
        // automatic block updates
        if(currentTimeMillis() - moveTime >= 1000){
            // move block down, if at bottom create new block, check filled line
            if(moveDown() == 0){
                // check loss
                for(int i = 0; i < 4; i++){
                    if(yPos[i] == 1) play = 0;
                }
                // check filled lines
                for(int i = 0; i <= screenSize; i++){
                    if(screen[i] == '[' || screen[i] == ']'){
                        lineCount++;
                    }
                    else lineCount = 0;

                    if(lineCount == screenWidth - 2){
                        for(int j = i; j > screenWidth; j--){
                            screen[j] = screen[j - screenWidth];
                        }
                        lines++;
                    }
                }

                newBlock(rand() % (6 + 1));
            }
        }

        // user input
        if(kbhit()){ 
            ch=getch();
            if(ch == 72){   // up
                rotate();
            }
            else if(ch == 80){  // down
                moveDown();
            }
            else if(ch == 75 || ch == 77){  // left or right
                clearBlock();

                if(ch == 75){ // left
                    moveFlag = 1;
                    for(int x = 0; x < 4; x++){
                        if(screen[(screenWidth * yPos[x] + (xPos[x]) - 2)] != ' '){
                            moveFlag = 0;   // if none can move
                        }
                    }

                    if(moveFlag){
                        for(int x = 0; x < 4; x++){
                            xPos[x] -= 2;
                        }
                    }
                }

                else{  //right
                    moveFlag = 1;
                    for(int x = 0; x < 4; x++){
                        if(screen[(screenWidth * yPos[x] + (xPos[x]) + 2)] != ' '){
                            moveFlag = 0;   // if none can move
                        }
                    }

                    if(moveFlag){
                        for(int x = 0; x < 4; x++){
                            xPos[x] += 2;
                        }
                    }
                }

                drawBlock();
            }

            else if(ch == 32){      // new block on space
                //newBlock()
            }

            else{
                //printf("%d", ch);
            }
        }

        
    }

    printf("\nYou Lose! Lines: %d\e[?25h", lines);

    return 0;
}