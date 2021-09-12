SFML_INCLUDE_DIR="/usr/local/Cellar/sfml/2.5.1_1/include"
SFML_LIB_DIR="/usr/local/Cellar/sfml/2.5.1_1/lib"
SFML_LIBRARIES="-lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio"
mkdir -p build
cd build
clang++ -std=c++17 -Wall -c ../game.cpp ../main.cpp -I${SFML_INCLUDE_DIR}
clang++ main.o game.o -o xonix -L${SFML_LIB_DIR} ${SFML_LIBRARIES}
cp xonix ../xonix
cd ..