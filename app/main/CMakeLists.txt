idf_component_register(SRCS "app.c"
                    INCLUDE_DIRS "." "../services"
                    SRCS "../services/wifi.c" "../services/http_client.c" "../services/http_server.c"
                    )