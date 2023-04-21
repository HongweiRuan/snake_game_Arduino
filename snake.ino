#include <gamma.h>
#include <RGBmatrixPanel.h>
#include <Adafruit_GFX.h>

// define the wiring of the LED screen
const uint8_t CLK = 8;
const uint8_t LAT = A3;
const uint8_t OE = 9;
const uint8_t A = A0;
const uint8_t B = A1;
const uint8_t C = A2;

// define the wiring of the inputs
const int POTENTIOMETER_PIN_NUMBER = 5;
const int BUTTON_PIN_NUMBER = 10;

// a global variable that represents the LED screen
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);

// define Delay
const int SNAKE_DELAY = 400;
const int FOOD_DELAY = 100;

// define Delay
const int max_num_of_food = 6;
int max_length = 100;
int level = 1;
int overall_scores = 0;

byte x[40];
byte y[40];

int directionThresholdMin = 341;  //
int directionThresholdMax = 683;  //

// the following functions are for printing messages
void print_level();
void print_scores();
void game_over();

class Food;
class Game;



Food food_generate();

class Color {
public:
  int red;
  int green;
  int blue;
  Color() {
    red = 0;
    green = 0;
    blue = 0;
  }
  Color(int r, int g, int b) {
    red = r;
    green = g;
    blue = b;
  }
  void set(Color color) {
    red = color.red;
    green = color.green;
    blue = color.blue;
  }
  uint16_t to_333() const {
    return matrix.Color333(red, green, blue);
  }
};

const Color BLACK(0, 0, 0);
const Color RED(4, 0, 0);
const Color ORANGE(6, 1, 0);
const Color YELLOW(4, 4, 0);
const Color GREEN(0, 4, 0);
const Color BLUE(0, 0, 4);
const Color PURPLE(1, 0, 2);
const Color WHITE(4, 4, 4);
const Color LIME(2, 4, 0);
const Color AQUA(0, 4, 4);

class Food {

public:

  Food() {
    x = 0;
    y = 0;
    score = 0;
    color.set(RED);
    time = millis();
  }

  Food(int x_arg, int y_arg, int score_arg, Color color_arg) {
    x = x_arg;
    y = y_arg;
    score = score_arg;
    color.set(color_arg);
  }

  // getters
  int get_x() const {
    return x;
  }
  int get_y() const {
    return y;
  }
  int get_score() const {
    return score;
  }
  Color get_color() const {
    return color;
  }

  void draw(int x, int y, Color color) {
    matrix.drawPixel(x, y, color.to_333());
  }

  unsigned long getUnsignedTime() {
    return time;
  }

private:
  int x;
  int y;
  Color color;
  int score;
  unsigned long time;
};

// a gloabl variable used to store food
Food food[5];


class Snake {
public:

  Snake() {
    length = 4;       // Initial length of the snake
    direction = 'e';  // The snake starts moving towards the east

    // Set the initial position of the snake segments
    x[0] = 10;
    y[0] = 8;  // Head
    x[1] = 9;
    y[1] = 8;  // Second segment
    x[2] = 8;
    y[2] = 8;  // Third segment
    x[3] = 7;
    y[3] = 8;  // Tail

    time = millis();
  }

  int get_length() {
    return length;
  }

  void erase(int x, int y) {
    matrix.drawPixel(x, y, BLACK.to_333());
  }

  void move(int d_flag) {
    if ((millis() - time) > SNAKE_DELAY) {
      int copy_last_x = 0;
      int copy_last_y = 0;
      copy_last_x = x[length - 1];
      copy_last_y = y[length - 1];

      for (int i = length - 1; i > 0; i--) {
        x[i] = x[i - 1];
        y[i] = y[i - 1];
      }

      if (d_flag == 0) {
        if (direction == 'w') {
          x[0] -= 1;
          direction = 'w';
        } else if (direction == 'n') {
          y[0] -= 1;
          direction = 'n';
        } else if (direction == 'e') {
          x[0] += 1;
          direction = 'e';
        } else {
          y[0] += 1;
          direction = 's';
        }

      } else if (d_flag == 1) {  // Turn right
        if (direction == 'w') {
          y[0] -= 1;
          direction = 'n';
        } else if (direction == 'n') {
          x[0] += 1;
          direction = 'e';
        } else if (direction == 'e') {
          y[0] += 1;
          direction = 's';
        } else {
          x[0] -= 1;
          direction = 'w';
        }
      } else if (d_flag == 2) {  // Turn left
        if (direction == 'w') {
          y[0] += 1;
          direction = 's';
        } else if (direction == 'n') {
          x[0] -= 1;
          direction = 'w';
        } else if (direction == 'e') {
          y[0] -= 1;
          direction = 'n';
        } else {
          x[0] += 1;
          direction = 'e';
        }
      }

      if (collide_with_food()) {
        eat_food();
        grow();
      }

      draw();
      erase(copy_last_x, copy_last_y);

      time = millis();  // Update the time variable after the snake has moved
    }
  }


  bool collide_with_itself() {  // x_arg and y_arg is the coordinate of snake head
    bool flag = false;
    for (int i = 1; i <= length; i++) {
      if (x[i] == x[0] && y[i] == y[0]) {
        flag = true;
      }
    }
    return flag;
  }

  bool collide_with_wall() {
    bool flag = false;
    if (x[0] <= -1 || y[0] <= -1 || x[0] >= 32 || y[0] >= 16) {
      flag = true;
    }
    return flag;
  }

  bool collide_with_food() {
    for (int i = 0; i < max_num_of_food; i++) {
      if (food[i].get_x() == x[0] && food[i].get_y() == y[0]) {
        return true;
      }
    }
    return false;
  }

  void eat_food() {
    for (int i = 0; i < max_num_of_food; i++) {
      if (food[i].get_x() == x[0] && food[i].get_y() == y[0]) {
        overall_scores += food[i].get_score();
        food[i] = food_generate();
        food[i].draw(food[i].get_x(), food[i].get_y(), food[i].get_color());
        break;
      }
    }
  }

  void grow() {
    if (length < max_length) {
      length++;
      x[length - 1] = x[length - 2];
      y[length - 1] = y[length - 2];
    }
  }


  void draw() {
    for (int i = 0; i < length; i++) {
      matrix.drawPixel(x[i], y[i], ORANGE.to_333());
    }
  }

private:
  int length;
  char direction;
  unsigned long time;
};

Snake snake;

Food food_generate() {
  int flag = 0;
  int food_x = 0;
  int food_y = 0;
  while (flag == 0) {
    food_x = rand() % 32;
    food_y = rand() % 16;

    flag = 1;  // Set flag to 1 before the for loops

    for (int i = 0; i < max_num_of_food; i++) {
      if (food[i].get_x() == food_x && food[i].get_y() == food_y) {
        flag = 0;
        break;
      }
    }


    // Check for overlap with the snake only if no overlap with food
    if (flag == 1) {
      for (int i = 0; i < snake.get_length(); i++) {
        if (x[i] == food_x && y[i] == food_y) {
          flag = 0;
          break;
        }
      }
    }
    if (flag == 1) {
      if (food_x == 0 || food_x == 31 || food_y == 0 || food_y == 15) {
        flag = 0;
      }
    }
  }
  int type = rand() % 10;
  int score = 0;
  Color color;
  if (type < level) {
    color.set(PURPLE);
    score = -200;
  } else {
    type = rand() % 3;
    if (type == 0) {
      color.set(GREEN);
      score = 100;
    } else if (type == 1) {
      color.set(LIME);
      score = 200;
    } else if (type == 2) {
      color.set(BLUE);
      score = 300;
    }
  }

  Food food(food_x, food_y, score, color);

  return food;
}

class Game {
public:

  Game() {
    snake = Snake();
    level = 1;
    time = 0;
  }

  void setupGame() {
    matrix.fillScreen(BLACK.to_333());
    delay(50);
    display_status();

    for (int i = 0; i < 6; i++) {
      food[i] = food_generate();
    }

    // resetSnake();
    // Draw the snake after initializing the snake
    snake.draw();

    // Draw the food after initializing the snake
    food[0].draw(food[0].get_x(), food[0].get_y(), food[0].get_color());
    delay(500);
    food[1].draw(food[1].get_x(), food[1].get_y(), food[1].get_color());
    delay(500);
    food[2].draw(food[2].get_x(), food[2].get_y(), food[2].get_color());
    delay(500);
    food[3].draw(food[3].get_x(), food[3].get_y(), food[3].get_color());
    delay(500);
    food[4].draw(food[4].get_x(), food[4].get_y(), food[4].get_color());
    delay(500);
    food[5].draw(food[5].get_x(), food[5].get_y(), food[5].get_color());
    delay(500);
  }

  void update(int potentiometer_value, bool button_pressed) {
    if (overall_scores < 5000) {
      int d_flag = 0;
      if (directionThresholdMin <= potentiometer_value && potentiometer_value <= directionThresholdMax) {
        d_flag = 0;
      } else if (potentiometer_value > directionThresholdMax) {
        d_flag = 1;
      } else if (potentiometer_value < directionThresholdMin) {
        d_flag = 2;
      }

      snake.move(d_flag);
      Serial.print(level);


      if (snake.collide_with_itself() || snake.collide_with_wall()) {
        game_over();
        print_scores();
        exit(0);
      }

      int prev_level = level;

      if (overall_scores < 800) {
        level = 1;
      } else if (800 <= overall_scores && overall_scores < 1500) {
        level = 2;
      } else if (1500 <= overall_scores && overall_scores < 2000) {
        level = 3;
      } else if (2000 <= overall_scores && overall_scores < 2500) {
        level = 4;
      } else if (2500 <= overall_scores && overall_scores < 3000) {
        level = 5;
      }

      if (level != prev_level) {
        // Only update the food and redraw the snake when the level changes
        setupGame();
      }
    }

    else {
      game_win();
      print_scores();
      exit(0);
    }
  }

private:

  unsigned long time;

  void display_status() {
    matrix.fillScreen(BLACK.to_333());
    matrix.setCursor(0, 0);
    print_level();
    delay(1000);
    matrix.fillScreen(BLACK.to_333());
    delay(500);
    matrix.setCursor(0, 0);
    print_scores();
    delay(1000);
    matrix.fillScreen(BLACK.to_333());
  }
};

Game game;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //randomSeed(analogRead(4));
  pinMode(BUTTON_PIN_NUMBER, INPUT);
  matrix.begin();
  game.setupGame();
}

void loop() {
  // put your main code here, to run repeatedly:
  int potentiometer_value = analogRead(POTENTIOMETER_PIN_NUMBER);
  bool button_pressed = (digitalRead(BUTTON_PIN_NUMBER) == HIGH);
  game.update(potentiometer_value, button_pressed);
}

void print_level() {
  matrix.setCursor(0, 0);
  matrix.print("Level:");
  delay(1000);
  matrix.print(level);
  delay(1000);
  matrix.fillScreen(BLACK.to_333());
}

void print_scores() {
  matrix.setCursor(0, 0);
  matrix.print("Score:");
  delay(1000);
  matrix.print(overall_scores);
  delay(1000);
  matrix.fillScreen(BLACK.to_333());
}

void game_over() {
  matrix.fillScreen(BLACK.to_333());
  matrix.setCursor(0, 0);
  matrix.println("LOSE");
  matrix.print(":(");
  delay(2000);
  matrix.fillScreen(BLACK.to_333());
}

void game_win() {
  matrix.fillScreen(BLACK.to_333());
  matrix.setCursor(0, 0);
  matrix.print("WIN! :)");
  delay(20000);
  matrix.fillScreen(BLACK.to_333());
}