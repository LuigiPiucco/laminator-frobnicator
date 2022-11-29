#ifndef SERIAL_LOGGING_H_
#define SERIAL_LOGGING_H_

// Caso estejamos em modo de otimização, não queremos acessos à serial, ela
// provavelmente nem está conectada. Assim, tranformamos as macros de logging
// para expanções vazias.
#ifdef NDEBUG

#define serial_begin(...)
#define serial_print(...)
#define serial_println(...)

#else

#define serial_begin(...) Serial.begin(__VA_ARGS__)
#define serial_print(...) Serial.print(__VA_ARGS__)
#define serial_println(...) Serial.println(__VA_ARGS__)

#endif

#endif // SERIAL_LOGGING_H_
