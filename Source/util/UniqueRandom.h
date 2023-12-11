#include <stdio.h> 
#include <stdlib.h> 
#include <time.h>   

class UniqueRandom {
private:
  int* numbers;
  int size = 0;
  int repetitionBuffer;
  int rightSideIndex;
  int border;

public:
  UniqueRandom(int min, int max, int cooldown): repetitionBuffer(cooldown) {
    srand(time(NULL));
    setSize(min, max, cooldown);
  }

  void setSize(int min, int max, int cooldown) {
    size = max - min + 1;
    numbers = new int[size];

    int cooldownIndex = size - 1 - cooldown;
    border = std::max(size - 1 - cooldown, 0);

    for (int i = 0; i < size; i++)
      numbers[i] = i + min;
  }

  int next() {
    // clamp a number between 0 and size
    int index = rand() % border;
    int value = numbers[index];
    // swap the value at the random index with the last index
    int rightSideIndex = getRightSideIndex();
    numbers[index] = numbers[rightSideIndex];
    numbers[rightSideIndex] = value;

    return value;
  }

  int getRightSideIndex() {
    rightSideIndex = std::min(rightSideIndex + 1, size - 1 - repetitionBuffer);
    return rightSideIndex;
  }
};
