#include <driver/i2s.h>

// you shouldn't need to change these settings
#define SAMPLE_BUFFER_SIZE 512
#define SAMPLE_RATE 8000

// En-tête WAV
const uint8_t wavHeader[] PROGMEM = {
  // RIFF Chunk
  'R', 'I', 'F', 'F', // ChunkID
  0x74, 0x08, 0x00, 0x00, // ChunkSize (2084 en little-endian : 0x00000874)
  'W', 'A', 'V', 'E', // Format

  // fmt Subchunk
  'f', 'm', 't', ' ', // Subchunk1ID
  0x10, 0x00, 0x00, 0x00, // Subchunk1Size (16)
  0x01, 0x00, // AudioFormat (1 = PCM)
  0x01, 0x00, // NumChannels (1)
  0x00, 0x3E, 0x00, 0x00, // SampleRate (16000 en little-endian : 0x00003E80)
  0x00, 0x7D, 0x00, 0x00, // ByteRate (32000 en little-endian : 0x00007D00)
  0x02, 0x00, // BlockAlign (2)
  0x10, 0x00, // BitsPerSample (16)

  // data Subchunk
  'd', 'a', 't', 'a', // Subchunk2ID
  0x00, 0x08, 0x00, 0x00  // Subchunk2Size (2048 en little-endian : 0x00000800)
};

// don't mess around with this
i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0};

// and don't mess around with this
i2s_pin_config_t i2s_mic_pins = {
    .bck_io_num = PIN_MIC_BCLK,
    .ws_io_num = PIN_MIC_LRCLK,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = PIN_MIC_DOUT};


Micro::Micro(int pinData, int pinClk, int pinLRClk)
{
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);

  i2s_mic_pins.data_out_num = I2S_PIN_NO_CHANGE,
  i2s_mic_pins.data_in_num = pinData;
  i2s_mic_pins.bck_io_num = pinClk;
  i2s_mic_pins.ws_io_num = pinLRClk;
  i2s_set_pin(I2S_NUM_0, &i2s_mic_pins);
}

int32_t raw_samples[SAMPLE_BUFFER_SIZE];

void Micro::Read()
{
  // read from the I2S device
  size_t bytes_read = 0;
  i2s_read(I2S_NUM_0, raw_samples, sizeof(int32_t) * SAMPLE_BUFFER_SIZE, &bytes_read, portMAX_DELAY);
  int samples_read = bytes_read / sizeof(int32_t);
}

void Micro::GetData()
{

}

void Micro::GetWave()
{
// Envoyer l'en-tête WAV
//  for (uint8_t i = 0; i < sizeof(wavHeader); i++) {
//    Serial.write(pgm_read_byte_near(wavHeader + i));
//  }

  // Envoyer les données audio
//  for (uint16_t i = 0; i < sizeof(audioData); i++) {
//    Serial.write(pgm_read_byte_near(audioData + i));
//  }  
}