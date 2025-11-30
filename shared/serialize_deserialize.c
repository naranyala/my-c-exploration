/**
 * Binary serialization/deserialization system
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char name[32];
  int age;
  float height;
  uint8_t flags;
} Person;

typedef struct {
  uint32_t magic;
  uint32_t version;
  uint32_t data_size;
  uint32_t checksum;
} FileHeader;

uint32_t calculate_checksum(const void *data, size_t size) {
  const uint8_t *bytes = (const uint8_t *)data;
  uint32_t checksum = 0;

  for (size_t i = 0; i < size; i++) {
    checksum = (checksum << 5) + checksum + bytes[i];
  }

  return checksum;
}

int serialize_person(const Person *person, const char *filename) {
  FILE *file = fopen(filename, "wb");
  if (!file) {
    perror("Failed to open file for writing");
    return 0;
  }

  FileHeader header;
  header.magic = 0x4D414749; // "MAGI"
  header.version = 1;
  header.data_size = sizeof(Person);
  header.checksum = calculate_checksum(person, sizeof(Person));

  // Write header
  if (fwrite(&header, sizeof(FileHeader), 1, file) != 1) {
    fclose(file);
    return 0;
  }

  // Write data
  if (fwrite(person, sizeof(Person), 1, file) != 1) {
    fclose(file);
    return 0;
  }

  fclose(file);
  return 1;
}

int deserialize_person(Person *person, const char *filename) {
  FILE *file = fopen(filename, "rb");
  if (!file) {
    perror("Failed to open file for reading");
    return 0;
  }

  FileHeader header;
  if (fread(&header, sizeof(FileHeader), 1, file) != 1) {
    fclose(file);
    return 0;
  }

  // Verify magic number
  if (header.magic != 0x4D414749) {
    fprintf(stderr, "Invalid file format\n");
    fclose(file);
    return 0;
  }

  // Verify data size
  if (header.data_size != sizeof(Person)) {
    fprintf(stderr, "Data size mismatch\n");
    fclose(file);
    return 0;
  }

  // Read data
  if (fread(person, sizeof(Person), 1, file) != 1) {
    fclose(file);
    return 0;
  }

  // Verify checksum
  uint32_t actual_checksum = calculate_checksum(person, sizeof(Person));
  if (header.checksum != actual_checksum) {
    fprintf(stderr, "Checksum verification failed\n");
    fclose(file);
    return 0;
  }

  fclose(file);
  return 1;
}

// Example usage
int main(void) {
  Person person = {
      .name = "John Doe", .age = 30, .height = 1.75f, .flags = 0x03};

  // Serialize
  if (serialize_person(&person, "person.dat")) {
    printf("Person serialized successfully\n");
  }

  // Deserialize
  Person loaded_person;
  if (deserialize_person(&loaded_person, "person.dat")) {
    printf("Person deserialized successfully:\n");
    printf("Name: %s\n", loaded_person.name);
    printf("Age: %d\n", loaded_person.age);
    printf("Height: %.2f\n", loaded_person.height);
    printf("Flags: 0x%02X\n", loaded_person.flags);
  }

  return 0;
}
