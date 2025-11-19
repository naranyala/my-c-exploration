/**
 * Object-oriented programming in C using structs and function pointers
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Base "class"
typedef struct Animal Animal;

typedef struct {
  void (*speak)(Animal *);
  void (*destroy)(Animal *);
} AnimalVTable;

struct Animal {
  AnimalVTable vtable;
  char name[32];
  int age;
};

// Derived "class": Dog
typedef struct {
  Animal base;
  char breed[32];
} Dog;

void animal_speak(Animal *animal) {
  printf("Animal %s makes a sound\n", animal->name);
}

void animal_destroy(Animal *animal) {
  printf("Destroying animal: %s\n", animal->name);
  free(animal);
}

void dog_speak(Animal *animal) {
  Dog *dog = (Dog *)animal;
  printf("Dog %s (%s) says: Woof!\n", dog->base.name, dog->breed);
}

void dog_destroy(Animal *animal) {
  printf("Destroying dog: %s\n", animal->name);
  free(animal);
}

Animal *animal_create(const char *name, int age) {
  Animal *animal = malloc(sizeof(Animal));
  if (!animal)
    return NULL;

  animal->vtable.speak = animal_speak;
  animal->vtable.destroy = animal_destroy;
  strncpy(animal->name, name, sizeof(animal->name) - 1);
  animal->age = age;

  return animal;
}

Animal *dog_create(const char *name, int age, const char *breed) {
  Dog *dog = malloc(sizeof(Dog));
  if (!dog)
    return NULL;

  dog->base.vtable.speak = dog_speak;
  dog->base.vtable.destroy = dog_destroy;
  strncpy(dog->base.name, name, sizeof(dog->base.name) - 1);
  dog->base.age = age;
  strncpy(dog->breed, breed, sizeof(dog->breed) - 1);

  return (Animal *)dog;
}

// Container for polymorphism
typedef struct {
  Animal **animals;
  int count;
  int capacity;
} Zoo;

Zoo *zoo_create(int capacity) {
  Zoo *zoo = malloc(sizeof(Zoo));
  if (!zoo)
    return NULL;

  zoo->animals = malloc(capacity * sizeof(Animal *));
  if (!zoo->animals) {
    free(zoo);
    return NULL;
  }

  zoo->count = 0;
  zoo->capacity = capacity;
  return zoo;
}

void zoo_add_animal(Zoo *zoo, Animal *animal) {
  if (zoo->count < zoo->capacity) {
    zoo->animals[zoo->count++] = animal;
  }
}

void zoo_make_sounds(Zoo *zoo) {
  for (int i = 0; i < zoo->count; i++) {
    zoo->animals[i]->vtable.speak(zoo->animals[i]);
  }
}

void zoo_destroy(Zoo *zoo) {
  for (int i = 0; i < zoo->count; i++) {
    zoo->animals[i]->vtable.destroy(zoo->animals[i]);
  }
  free(zoo->animals);
  free(zoo);
}

// Example usage
int main(void) {
  Zoo *zoo = zoo_create(10);

  // Create different types of animals
  Animal *generic_animal = animal_create("Generic", 2);
  Animal *dog1 = dog_create("Buddy", 3, "Golden Retriever");
  Animal *dog2 = dog_create("Max", 5, "German Shepherd");

  // Add to zoo (polymorphism)
  zoo_add_animal(zoo, generic_animal);
  zoo_add_animal(zoo, dog1);
  zoo_add_animal(zoo, dog2);

  // Make sounds (polymorphic behavior)
  zoo_make_sounds(zoo);

  zoo_destroy(zoo);
  return 0;
}
