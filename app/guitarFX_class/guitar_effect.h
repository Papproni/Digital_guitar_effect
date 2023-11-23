

typedef struct param_pot_st{
    uint8_t pot1;
    uint8_t pot2;
    uint8_t pot3;
    uint8_t pot4;
    uint8_t pot5;
    uint8_t pot6;
}param_pot_tst;

struct guitarFX{
    param_pot_tst param_st;

    void *(void*)signal_process;
}