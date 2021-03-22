# ESP32 D1 Mini + TFT_eSPI lib

La configuration des pins est définie dans [include/User_Setup.h](include/User_Setup.h). Pour qu'elle soit correctement prise en compte par la lib, il faut :

1. Lancer un build afin que PlatfomIO télécharge la lib TFT_eSPI dans le répertoire `.pio/libdeps/wemos_d1_mini32/TFT_eSPI`.

2. Deux possibilités :
    * Copier le contenu de `include/User_Setup.h` dans `.pio/libdeps/wemos_d1_mini32/TFT_eSPI/User_Setup.h` afin d'écraser la configuration par défaut.
    * Supprimer le fichier `.pio/libdeps/wemos_d1_mini32/TFT_eSPI/User_Setup.h`, le compilateur utilisera alors le fichier `include/User_Setup.h` qui est dans l'_include path_.

3. Relancer le build afin d'utiliser la configuration à jour.