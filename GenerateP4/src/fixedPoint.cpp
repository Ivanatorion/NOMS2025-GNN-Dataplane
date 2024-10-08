#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cmath>

typedef uint32_t FixedPointT;

FixedPointT convert(const double input, const int precision){
    return (FixedPointT)(input * (1 << precision));
}

double convert(const FixedPointT input, const int precision){
    double result = 0;
    
    for(int i = 0; i < 31; i++)
        if(((input >> i) & 1) == 1)
            result = result + pow(2.0, static_cast<double>(i - precision));

    if(((input >> 31) & 1) == 1)
        result = result - pow(2.0, static_cast<double>(31 - precision));

    return result;
}

FixedPointT fromBinStr(char str[]){
    FixedPointT result = 0;

    for(int i = 0; i < 32; i++)
        if(str[31 - i] == '1')
            result = result | (1 << i);
    
    return result;
}

void toBinStr(const FixedPointT input, char str[]){
    str[0] = '0';
    str[1] = 'b';
    for(int i = 0; i < 32; i++){
        if(((input >> (31 - i)) & 1) == 1)
            str[i+2] = '1';
        else
            str[i+2] = '0';
    }
    str[34] = '\0';
}

int intFromString(char str[]){
    int result = 0;
    int i = 0;
    while(str[i] != '\0'){
        if(str[i] < '0' || str[i] > '9')
            return -1;
        
        result = result * 10 + (str[i] - '0');
        i++;
    }
    return result;
}

bool isDouble(char str[]){
    bool point = false;
    int i = (str[0] == '-') ? 1 : 0;
    while(str[i] != '\0'){
        if(str[i] == '.'){
            if(point)
                return false;
            point = true;
        }
        else if(str[i] < '0' || str[i] > '9')
            return false;

        i++;
    }
    return true;
}

int main(int argc, char *argv[]){
    if(argc != 3){
        printf("Usage: %s PRECISION (32_BIT_FIXED_POINT_BINARY_NUMBER | DOUBLE)\n", argv[0]);
        return 1;
    }

    const int precision = intFromString(argv[1]);
    if(precision < 0 || precision > 32){
        fprintf(stderr, "Invalid precision \"%s\"\n", argv[1]);
        return 1;
    }

    if(strlen(argv[2]) == 32) {
        printf("%lf\n", convert(fromBinStr(argv[2]), precision));
    }
    else if(strlen(argv[2]) == 34 && argv[2][0] == '0' && argv[2][1] == 'b') {
        printf("%lf\n", convert(fromBinStr(argv[2] + 2), precision));
    }
    else if(isDouble(argv[2])){
        char str[64];
        toBinStr(convert(atof(argv[2]), precision), str);
        printf("%s\n", str);
    }
    else{
        printf("Invalid number \"%s\"\n", argv[2]);
        return 1;
    }

    return 0;
}
