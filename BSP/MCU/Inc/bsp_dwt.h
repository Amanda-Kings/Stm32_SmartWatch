#ifndef BSP_DWT_H
#define BSP_DWT_H

#include "stm32f10x_conf.h"

// DWT 寄存器定义
typedef struct {
  __IO uint32_t CTRL;
  __IO uint32_t CYCCNT;
  __IO uint32_t CPICNT;
  __IO uint32_t EXCCNT;
  __IO uint32_t SLEEPCNT;
  __IO uint32_t LSUCNT;
  __IO uint32_t FOLDCNT;
  __I  uint32_t PCSR;
} DWT_TypeDef;

#define DWT_CTRL_CYCCNTENA_Pos              0                                             /*!< DWT CTRL: CYCCNTENA Position */
#define DWT_CTRL_CYCCNTENA_Msk             (1ul << DWT_CTRL_CYCCNTENA_Pos)                    /*!< DWT CTRL: CYCCNTENA Mask */


// 基地址定义
#define DWT_BASE            (0xE0001000UL)
#define DWT                 ((DWT_TypeDef *)DWT_BASE)

void DWT_Init(void);

#endif