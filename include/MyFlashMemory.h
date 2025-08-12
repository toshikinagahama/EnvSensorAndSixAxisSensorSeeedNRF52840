#include <Arduino.h>
#include <nrf52840.h>
#include "nrfx_qspi.h"
#include "app_util_platform.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "sdk_config.h"
#include "nrf_delay.h"
#include "avr/interrupt.h"

// QSPI Settings
#define QSPI_STD_CMD_WRSR 0x01
#define QSPI_STD_CMD_RSTEN 0x66
#define QSPI_STD_CMD_RST 0x99
#define QSPI_DPM_ENTER 0x0003 // 3 x 256 x 62.5ns = 48ms
#define QSPI_DPM_EXIT 0x0003

static uint32_t *QSPI_Status_Ptr = (uint32_t *)0x40029604; // Setup for the SEEED XIAO BLE - nRF52840
static nrfx_qspi_config_t QSPIConfig;
static nrf_qspi_cinstr_conf_t QSPICinstr_cfg;
static const uint32_t MemToUse = 64 * 1024; // Alter this to create larger read writes, 64Kb is the size of the Erase
static bool Debug_On = true;
static uint16_t pBuf[MemToUse / 2] = {0}; // 16bit used as that is what this memory is going to be used for
static uint32_t *BufMem = (uint32_t *)&pBuf;
static bool QSPIWait = false;
// QSPI Settings Complete

static void qspi_handler(nrfx_qspi_evt_t event, void *p_context)
{
  // UNUSED_PARAMETER(p_context);
  // Serial.println("QSPI Interrupt");
  // if (event == NRFX_QSPI_EVENT_DONE) {
  //   QSPI_HasFinished = true;
  // }
}

static void QSPI_Status(char ASender[])
{ // Prints the QSPI Status
  Serial.print("(");
  Serial.print(ASender);
  Serial.print(") QSPI is busy/idle ... Result = ");
  Serial.println(nrfx_qspi_mem_busy_check() & 8);
  Serial.print("(");
  Serial.print(ASender);
  Serial.print(") QSPI Status flag = 0x");
  Serial.print(NRF_QSPI->STATUS, HEX);
  Serial.print(" (from NRF_QSPI) or 0x");
  Serial.print(*QSPI_Status_Ptr, HEX);
  Serial.println(" (from *QSPI_Status_Ptr)");
}

static void QSPI_PrintData(uint16_t *AnAddress, uint32_t AnAmount)
{
  uint32_t i;

  Serial.print("Data :");
  for (i = 0; i < AnAmount; i++)
  {
    Serial.print(" 0x");
    Serial.print(*(AnAddress + i), HEX);
  }
  Serial.println("");
}

static nrfx_err_t QSPI_IsReady()
{
  if (((*QSPI_Status_Ptr & 8) == 8) && (*QSPI_Status_Ptr & 0x01000000) == 0)
  {
    return NRFX_SUCCESS;
  }
  else
  {
    return NRFX_ERROR_BUSY;
  }
}

static nrfx_err_t QSPI_WaitForReady()
{
  while (QSPI_IsReady() == NRFX_ERROR_BUSY)
  {
    if (Debug_On)
    {
      Serial.print("*QSPI_Status_Ptr & 8 = ");
      Serial.print(*QSPI_Status_Ptr & 8);
      Serial.print(", *QSPI_Status_Ptr & 0x01000000 = 0x");
      Serial.println(*QSPI_Status_Ptr & 0x01000000, HEX);
      QSPI_Status("QSPI_WaitForReady");
    }
  }
  return NRFX_SUCCESS;
}

static void QSIP_Configure_Memory()
{
  // uint8_t  temporary = 0x40;
  uint8_t temporary[] = {0x00, 0x02};
  uint32_t Error_Code;

  QSPICinstr_cfg = {
      .opcode = QSPI_STD_CMD_RSTEN,
      .length = NRF_QSPI_CINSTR_LEN_1B,
      .io2_level = true,
      .io3_level = true,
      .wipwait = QSPIWait,
      .wren = true};
  QSPI_WaitForReady();
  if (nrfx_qspi_cinstr_xfer(&QSPICinstr_cfg, NULL, NULL) != NRFX_SUCCESS)
  { // Send reset enable
    if (Debug_On)
    {
      Serial.println("(QSIP_Configure_Memory) QSPI 'Send reset enable' failed!");
    }
  }
  else
  {
    QSPICinstr_cfg.opcode = QSPI_STD_CMD_RST;
    QSPI_WaitForReady();
    if (nrfx_qspi_cinstr_xfer(&QSPICinstr_cfg, NULL, NULL) != NRFX_SUCCESS)
    { // Send reset command
      if (Debug_On)
      {
        Serial.println("(QSIP_Configure_Memory) QSPI Reset failed!");
      }
    }
    else
    {
      QSPICinstr_cfg.opcode = QSPI_STD_CMD_WRSR;
      QSPICinstr_cfg.length = NRF_QSPI_CINSTR_LEN_3B;
      QSPI_WaitForReady();
      if (nrfx_qspi_cinstr_xfer(&QSPICinstr_cfg, &temporary, NULL) != NRFX_SUCCESS)
      { // Switch to qspi mode
        if (Debug_On)
        {
          Serial.println("(QSIP_Configure_Memory) QSPI failed to switch to QSPI mode!");
        }
      }
      else
      {
        QSPI_Status("QSIP_Configure_Memory");
      }
    }
  }
}

static nrfx_err_t QSPI_Initialise()
{ // Initialises the QSPI and NRF LOG
  uint32_t Error_Code;

  NRF_LOG_INIT(NULL); // Initialise the NRF Log
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  // QSPI Config
  QSPIConfig.xip_offset = NRFX_QSPI_CONFIG_XIP_OFFSET;
  QSPIConfig.pins = {
      // Setup for the SEEED XIAO BLE - nRF52840
      .sck_pin = 21,
      .csn_pin = 25,
      .io0_pin = 20,
      .io1_pin = 24,
      .io2_pin = 22,
      .io3_pin = 23,
  };
  QSPIConfig.irq_priority = (uint8_t)NRFX_QSPI_CONFIG_IRQ_PRIORITY;
  QSPIConfig.prot_if = {
      // .readoc     = (nrf_qspi_readoc_t)NRFX_QSPI_CONFIG_READOC,
      .readoc = (nrf_qspi_readoc_t)NRF_QSPI_READOC_READ4O,
      // .writeoc    = (nrf_qspi_writeoc_t)NRFX_QSPI_CONFIG_WRITEOC,
      .writeoc = (nrf_qspi_writeoc_t)NRF_QSPI_WRITEOC_PP4O,
      .addrmode = (nrf_qspi_addrmode_t)NRFX_QSPI_CONFIG_ADDRMODE,
      .dpmconfig = false,
  };
  QSPIConfig.phy_if.sck_freq = (nrf_qspi_frequency_t)NRF_QSPI_FREQ_32MDIV1; // I had to do it this way as it complained about nrf_qspi_phy_conf_t not being visible
  // QSPIConfig.phy_if.sck_freq   = (nrf_qspi_frequency_t)NRFX_QSPI_CONFIG_FREQUENCY;
  QSPIConfig.phy_if.spi_mode = (nrf_qspi_spi_mode_t)NRFX_QSPI_CONFIG_MODE;
  QSPIConfig.phy_if.dpmen = false;
  // QSPI Config Complete
  // Setup QSPI to allow for DPM but with it turned off
  QSPIConfig.prot_if.dpmconfig = true;
  NRF_QSPI->DPMDUR = (QSPI_DPM_ENTER << 16) | QSPI_DPM_EXIT; // Found this on the Nordic Q&A pages, Sets the Deep power-down mode timer
  Error_Code = 1;
  while (Error_Code != 0)
  {
    Error_Code = nrfx_qspi_init(&QSPIConfig, NULL, NULL);
    if (Error_Code != NRFX_SUCCESS)
    {
      if (Debug_On)
      {
        Serial.print("(QSPI_Initialise) nrfx_qspi_init returned : ");
        Serial.println(Error_Code);
      }
    }
    else
    {
      if (Debug_On)
      {
        Serial.println("(QSPI_Initialise) nrfx_qspi_init successful");
      }
    }
  }
  QSPI_Status("QSPI_Initialise (Before QSIP_Configure_Memory)");
  QSIP_Configure_Memory();
  if (Debug_On)
  {
    Serial.println("(QSPI_Initialise) Wait for QSPI to be ready ...");
  }
  NRF_QSPI->TASKS_ACTIVATE = 1;
  QSPI_WaitForReady();
  if (Debug_On)
  {
    Serial.println("(QSPI_Initialise) QSPI is ready");
  }
  return QSPI_IsReady();
}

static void QSPI_All_Erase()
{
  uint32_t TimeTaken;
  bool QSPIReady = false;
  bool AlreadyPrinted = false;

  if (Debug_On)
  {
    Serial.println("(QSPI_Erase) Erasing memory");
  }
  while (!QSPIReady)
  {
    if (QSPI_IsReady() != NRFX_SUCCESS)
    {
      if (!AlreadyPrinted)
      {
        QSPI_Status("QSPI_Erase (Waiting)");
        AlreadyPrinted = true;
      }
    }
    else
    {
      QSPIReady = true;
      QSPI_Status("QSPI_Erase (Waiting Loop Breakout)");
    }
  }
  if (Debug_On)
  {
    QSPI_Status("QSPI_Erase (Finished Waiting)");
    TimeTaken = millis();
  }
  if (nrfx_qspi_erase(NRF_QSPI_ERASE_LEN_ALL, 0) != NRFX_SUCCESS)
  {
    if (Debug_On)
    {
      Serial.print("(QSPI_Initialise_Page) QSPI Address 0x");
      Serial.print(0, HEX);
      Serial.println(" failed to erase!");
    }
  }
  else
  {
    if (Debug_On)
    {
      TimeTaken = millis() - TimeTaken;
      Serial.print("(QSPI_Initialise_Page) QSPI took ");
      Serial.print(TimeTaken);
      Serial.println("ms to erase a 64Kb page");
    }
  }
}

static void QSPI_Erase(uint32_t AStartAddress, nrf_qspi_erase_len_t Size)
{
  uint32_t TimeTaken;
  bool QSPIReady = false;
  bool AlreadyPrinted = false;

  if (Debug_On)
  {
    Serial.println("(QSPI_Erase) Erasing memory");
  }
  while (!QSPIReady)
  {
    if (QSPI_IsReady() != NRFX_SUCCESS)
    {
      if (!AlreadyPrinted)
      {
        QSPI_Status("QSPI_Erase (Waiting)");
        AlreadyPrinted = true;
      }
    }
    else
    {
      QSPIReady = true;
      QSPI_Status("QSPI_Erase (Waiting Loop Breakout)");
    }
  }
  if (Debug_On)
  {
    QSPI_Status("QSPI_Erase (Finished Waiting)");
    TimeTaken = millis();
  }
  if (nrfx_qspi_erase(Size, AStartAddress) != NRFX_SUCCESS)
  {
    if (Debug_On)
    {
      Serial.print("(QSPI_Initialise_Page) QSPI Address 0x");
      Serial.print(AStartAddress, HEX);
      Serial.println(" failed to erase!");
    }
  }
  else
  {
    if (Debug_On)
    {
      TimeTaken = millis() - TimeTaken;
      Serial.print("(QSPI_Initialise_Page) QSPI took ");
      Serial.print(TimeTaken);
      Serial.println("ms to erase a 64Kb page");
    }
  }
}

static void QSPI_Write(void *data, uint32_t AStartAddress, uint32_t Size)
{
  size_t aligned_length = (Size + 3) & ~3;
  uint8_t buffer[aligned_length];
  memcpy(buffer, data, Size);
  uint8_t pBuf2[4];
  // もとのデータで埋める
  nrfx_qspi_read(pBuf2, 4, AStartAddress);
  // Serial.print("Origin Data :");
  // for (uint8_t i = 0; i < 4; i++)
  //{
  //   Serial.print(" 0x");
  //   Serial.print((uint8_t)(pBuf2[i]), HEX);
  // }
  // Serial.println("");

  for (uint32_t i = Size; i < aligned_length; i++)
  {
    buffer[i] = pBuf2[i % 4];
  }
  // Serial.print("buffer :");
  // for (uint32_t i = 0; i < aligned_length; i++)
  //{
  //   Serial.print(" 0x");
  //   Serial.print(buffer[i], HEX);
  // }
  // Serial.println("");
  //  memset(buffer + Size, 0, aligned_length - Size);
  uint32_t Error_Code = nrfx_qspi_write(buffer, aligned_length, AStartAddress);
  // if (true)
  //{
  //   Serial.print(aligned_length);
  //   Serial.print("byte written, status = ");
  //   Serial.println(Error_Code);
  // }
}

static void MyFlashMemoryInitialize()
{
  QSPI_Initialise();
  // QSPI_All_Erase();
  QSPI_WaitForReady();
}