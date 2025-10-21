idf_component_register(SRCS "app.c"
                    INCLUDE_DIRS "." "../services"
                    EMBED_TXTFILES "httpbin_root_cert.pem"
                    )