#pragma once

#include "config.h"

/***
 *      ___                                   ___ _            
 *     / __| ___ __ _ _  _ ___ _ _  __ ___   / __| |__ _ ______
 *     \__ \/ -_) _` | || / -_) ' \/ _/ -_) | (__| / _` (_-<_-<
 *     |___/\___\__, |\_,_\___|_||_\__\___|  \___|_\__,_/__/__/
 *                 |_|                                         
 */

typedef struct sequenceDef {
    int (*step)(struct sequenceDef *);
    int config;
    int state;
} SequenceDef;


class Sequence {
    int stepNumber;
    byte sequenceConfig[SEQUENCE_COUNT];
    SequenceDef steps[STEP_COUNT];
    
public:
    Sequence();

    void SetConfig(byte *config);

    byte *GetConfig();
    
    int NextStep();

    int Tick();
};

/***
 *     __   __        _      _    _        
 *     \ \ / /_ _ _ _(_)__ _| |__| |___ ___
 *      \ V / _` | '_| / _` | '_ \ / -_|_-<
 *       \_/\__,_|_| |_\__,_|_.__/_\___/__/
 *                                         
 */
extern Sequence sequence;
