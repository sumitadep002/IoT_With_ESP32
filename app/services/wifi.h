#ifndef WIFI_H
#define WIFI_H

void wifi_init_apsta();
void wifi_init_ap();
void wifi_init_sta();
void wifi_state_set(bool state);
bool wifi_state_get();

#endif // WIFI_H