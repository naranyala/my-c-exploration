#include <stdio.h>

// Base struct with function pointers
typedef struct {
  int id;
  char name[50];
  void (*speak)(void *self);
} Animal;

// Dog-specific function
void dog_speak(void *self) {
  Animal *animal = (Animal *)self;
  printf("%s (Dog) says Woof!\n", animal->name);
}

// Cat-specific function
void cat_speak(void *self) {
  Animal *animal = (Animal *)self;
  printf("%s (Cat) says Meow!\n", animal->name);
}

// Initialize an animal with specific behavior
void init_dog(Animal *animal, int id, const char *name) {
  animal->id = id;
  snprintf(animal->name, sizeof(animal->name), "%s", name);
  animal->speak = dog_speak;
}

void init_cat(Animal *animal, int id, const char *name) {
  animal->id = id;
  snprintf(animal->name, sizeof(animal->name), "%s", name);
  animal->speak = cat_speak;
}

int main() {
  Animal animals[2];

  init_dog(&animals[0], 1, "Buddy");
  init_cat(&animals[1], 2, "Whiskers");

  for (int i = 0; i < 2; i++) {
    animals[i].speak(&animals[i]); // Polymorphic call
  }

  return 0;
}
