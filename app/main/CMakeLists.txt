idf_component_register(SRCS "app.c"
                    INCLUDE_DIRS "." "../services"
                    SRCS "../services/wifi.c" "../services/http.c"
                    EMBED_TXTFILES "httpbin_root_cert.pem"
                    )