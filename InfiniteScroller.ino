#include <AberLED.h>
#define MAXBULLETS 4
#define BULLETINTERVAL 100
#define SCROLLINTERVAL 1000
#define S_INVALID -1
#define S_START 0
#define S_PLAYING 1
#define S_LIFELOST 2
#define S_END 3
int xPlay;
int lifePlay;
int wall[8][8];
int xBullet[MAXBULLETS];
int yBullet[MAXBULLETS];
bool activeBullet[MAXBULLETS];
unsigned long lastBulletUpdateTime = 0;
unsigned long lastScroll = 0;
int state = S_INVALID;
unsigned long stateStartTime;
void setup() {
  AberLED.begin();
  Serial.begin(9600);
  initialisePlayer();
  wallInitialise();
  initBullet();
  State(S_START);
}
void myClear(){
 for(int x = 0; x < 8; x++){
   for(int y = 0; y < 8; y++){
    AberLED.set(x, y, BLACK);
  }
 }
}
void draw0(int colour) {
  for (int i = 0; i < 6; i++) {
    AberLED.set(i + 1, 0, colour);
    AberLED.set(i + 1, 1, colour);
    AberLED.set(i + 1, 6, colour);
    AberLED.set(i + 1, 7, colour);
  }
  for (int i = 0; i < 8; i++) {
    AberLED.set(1, i, colour);
    AberLED.set(2, i, colour);
    AberLED.set(5, i, colour);
    AberLED.set(6, i, colour);
  }
}
void draw1(int colour) {
  for (int i = 0; i < 8; i++) {
    AberLED.set(3, i, colour);
    AberLED.set(4, i, colour);
  }
}
void draw2(int colour) {
  AberLED.set(6, 2, colour);
  AberLED.set(5, 2, colour);
  AberLED.set(1, 5, colour);
  AberLED.set(2, 5, colour);
  for (int i = 0; i < 6; i++) {
    AberLED.set(i + 1, 0, colour);
    AberLED.set(i + 1, 1, colour);
    AberLED.set(i + 1, 3, colour);
    AberLED.set(i + 1, 4, colour);
    AberLED.set(i + 1, 6, colour);
    AberLED.set(i + 1, 7, colour);
  }
}
void draw3(int colour) {
  AberLED.set(6, 2, colour);
  AberLED.set(5, 2, colour);
  AberLED.set(6, 5, colour);
  AberLED.set(5, 5, colour);
  for (int i = 0; i < 6; i++) {
    AberLED.set(i + 1, 0, colour);
    AberLED.set(i + 1, 1, colour);
    AberLED.set(i + 1, 3, colour);
    AberLED.set(i + 1, 4, colour);
    AberLED.set(i + 1, 6, colour);
    AberLED.set(i + 1, 7, colour);
  }
}
void State(int s) {
  state = s;
  stateStartTime = millis();
}
unsigned long getStateTime() {
  return millis() - stateStartTime;
}
void initialisePlayer() {
  xPlay = 4;
  lifePlay = 3;
}
bool lifeFunctions() {
  lifePlay--;
  return lifePlay == 0;
}
void renderLives() {
  if (lifePlay == 2)
    draw2(GREEN);
  if (lifePlay == 1)
    draw1(YELLOW);
}
bool wallNplay() {
  return wall[xPlay][7] != 0;
}
void collisions() {
  if (wallNplay()) {
    if (lifeFunctions())
      State(S_END);
    else
      State(S_LIFELOST);
  }
}
void leftMove() {
  if (xPlay > 0)
    xPlay--;

}
void rightMove() {
  if (xPlay < 7)
    xPlay++;
}
bool wallPiece(int x, int y, bool unbreakable) {
  if (unbreakable)
    wall[x][y] = -1;
  else
    wall[x][y] = 2;
}
void scroll() {
  for (int y = 7; y > 0; y--) {
    for (int x = 0; x < 8; x++) {
      wall[x][y] = wall[x][y - 1];
    }

  }
}
void wall1stRow() {
  for (int x = 0; x < 8; x++) {
    if (random(10) == 0)
      wallPiece(x, 0, true);
    else
      wallPiece(x, 0, false);
  }
}
void wallInitialise() {
  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      wall[x][y] = 0;
    }
  }
}
void initBullet() {
  for (int i = 0; i < MAXBULLETS; i++)
    activeBullet[i] = false;
}
void fireBullet() {
  for (int i = 0; i < MAXBULLETS; i++) {
    if (!activeBullet[i]) {
      activeBullet[i] = true;
      xBullet[i] = xPlay;
      yBullet[i] = 7;
      return;
    }
  }
}
void updateBullet() {
  for (int i = 0; i < MAXBULLETS; i++) {
    if (activeBullet[i]) {
      if (wallNbullet(xBullet[i], yBullet[i]))
        activeBullet[i] = false;
      yBullet[i]--;
      if (yBullet[i] < 0)
        activeBullet[i] = false;
    }
  }
}
void wallExplode(int bx, int by) {
  for (int x = bx - 1; x <= bx + 1; x++) {
    for (int y = by - 1; y <= by + 1; y++) {
      if (x >= 0 && x < 8 && y >= 0 && y < 8) {
        if (wall[x][y] != 0 && random(3) != 0)
          wall[x][y]--;
      }
    }
  }
}
bool wallNbullet(int x, int y) {
  if (wall[x][y] != 0) {
    wall[x][y]--;
    if (wall[x][y] == 0)
      wallExplode(x, y);
    return true;
  }
  else
    return false;
}


void handleInput() {
  switch (state) {
    case S_START:
      if (AberLED.getButtonDown(5)) {
        initialisePlayer();
        wallInitialise();
        initBullet();
        State(S_PLAYING);
      }
      break;
    case S_PLAYING:
      if (AberLED.getButtonDown(3))
        rightMove();
      if (AberLED.getButtonDown(4))
        leftMove();
      if (AberLED.getButtonDown(5))
        fireBullet();
      break;
    case S_LIFELOST:
      break;
    case S_END:
      if (AberLED.getButtonDown(5))
        State(S_START);
      break;
    default:
      Serial.println("Bad state in update!");
      break;
  }
}


void update() {
  switch (state) {
    case S_START:
      break;
    case S_PLAYING:
      if (millis() - lastBulletUpdateTime > BULLETINTERVAL) {
        lastBulletUpdateTime = millis();
        updateBullet();
      }
      if (millis() - lastScroll > SCROLLINTERVAL) {
        lastScroll = millis();
        scroll();
        wall1stRow();
      }
      collisions();
      break;
    case S_LIFELOST:
      if (getStateTime() > 5000) {
        wallInitialise();
        initBullet();
        State(S_PLAYING);
      }
      break;
    case S_END:
      break;
    default:
      Serial.println("Bad state in Update!");
      break;
  }
}
void player() {
  AberLED.set(xPlay, 7, GREEN);
}
void wallDraw() {
  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      switch (wall[x][y]) {
        case 0: break;
        case 1:
        case 2:
          AberLED.set(x, y, RED); break;
        default:
          AberLED.set(x, y, YELLOW); break;
      }
    }
  }
}
void renderBullet() {
  for (int i = 0; i < MAXBULLETS; i++) {
    if (activeBullet[i])
      AberLED.set(xBullet[i], yBullet[i], GREEN);
  }
}
void renderBox(int colour) {
  for (int i = 0; i < 8; i++) {
    AberLED.set(0, i, colour);
    AberLED.set(7, i, colour);
    AberLED.set(i, 0, colour);
    AberLED.set(i, 7, colour);
  }
}


void render() {
  AberLED.clear();
  switch (state) {
    case S_START:
      renderBox(GREEN);
      break;
    case S_PLAYING:
      wallDraw();
      player();
      renderBullet();
      break;
    case S_LIFELOST:
      renderLives();
      break;
    case S_END:
    draw0(RED);
    default:
      Serial.println("Bad state in render!");
      break;
  }
  AberLED.swap();
}


void loop() {
  handleInput();
  update();
  render();

}
