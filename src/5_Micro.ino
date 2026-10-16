#include <driver/i2s.h>
//#include <ESP_I2S.h>

// you shouldn't need to change these settings
#define SAMPLE_RATE 16000
#define SAMPLE_BUFFER_SIZE SAMPLE_RATE // Total samples stored in memory, can be send via wifi
#define SAMPLE_TYPE int32_t

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

// and don't mess around with this
i2s_pin_config_t i2s_mic_pins = {
    .bck_io_num = PIN_MIC_BCLK,
    .ws_io_num = PIN_MIC_LRCLK,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = PIN_MIC_DOUT};


// don't mess around with this
i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = 0,
    .dma_buf_count = 16,
    .dma_buf_len = 512,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0};



Micro::Micro(int pinData, int pinClk, int pinLRClk)
{
  TRACE("Initialisation micro pinData:%d pinClk:%d pinLRCLK:%d\n", pinData, pinClk, pinLRClk);
  //I2S.setPins(pinClk, pinLRClk, -1, pinData); //SCK, WS, SDOUT (not used), SDIN
  //I2S.begin(I2S_MODE_STD, 16000, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO, -1, I2S_ROLE_SLAVE);
  
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);

  i2s_mic_pins.data_out_num = I2S_PIN_NO_CHANGE,
  i2s_mic_pins.data_in_num = pinData;
  i2s_mic_pins.bck_io_num = pinClk;
  i2s_mic_pins.ws_io_num = pinLRClk;
  i2s_set_pin(I2S_NUM_0, &i2s_mic_pins);


  this->currentPosition = 0;
  this->samples = new SAMPLE_TYPE [SAMPLE_BUFFER_SIZE];
  this->bufferLength = sizeof(SAMPLE_TYPE) * SAMPLE_BUFFER_SIZE;

  if (this->samples == NULL) {
    LOG_ERROR("Initialisation micro :ERREUR allocation buffer\n");
  } else {
    memset(this->samples, 0, this->bufferLength);
//  size_t read_bytes = I2S.readBytes((char *)this->samples, this->bufferLength);
//  TRACE("Micro::Micro() Test lecture -> %d octets", read_byte);
//  for(int i = 0; i < read_bytes; i++)
//    if (this->samples[i] != 0) TRACE("OK Ok OK OK OK OK -----------------------------")!
    TRACE("Initialisation micro : OK (buffer address %x) - size %d bytes\n", (int) this->samples, this->bufferLength);
  }
}


void Micro::Read()
{
  TRACE("Micro::Read()\n");
  bool c = true;

  while(c) {
    int currentPosition = this->currentPosition;
    // on reprend la lecture I2S là où on s'est arrêté
    TRACE("Micro::Read() Position %d - Data %x + %x\n", currentPosition, (int) this->samples, currentPosition);

    void *buffer = (void*) (this->samples + currentPosition);
    size_t maxSamples = this->bufferLength - this->currentPosition;
    size_t bytes_read = 0;
    TRACE("Micro::Read() Tentative de lecture de %d samples à l'adresse %x\n", maxSamples, buffer);
  

    // Effectue une lecture I2S, mais sans attendre (on lit tout ce qui est disponible)
    i2s_read(I2S_NUM_0, buffer, maxSamples, &bytes_read, 0); //, portMAX_DELAY);
//size_t read_bytes = I2S.readBytes((char *)buffer, maxSamples);
  
    bool ok = false;
    for (int  i = 0; (i < bytes_read) && (ok == false); i++)
    {
      uint8_t sample = ((uint8_t *) buffer)[i];
      if (sample != 0) ok = true; 
    }
    if (!ok) LOG_ERROR("Micro::Read() Aucun sample\ valide\n");
    
    // Détermine combien de samples ont été lus, et si on doit continuer
    currentPosition += bytes_read;
    TRACE("Micro::Read() Lus: %d octets soit %d samples\n", bytes_read, bytes_read / sizeof(SAMPLE_TYPE));
    
    if (currentPosition >= this->bufferLength)
      currentPosition = 0;

    this->currentPosition = currentPosition;
    // On a lu tout ce qu'on pouvait, on refait une passe
    c = (bytes_read == maxSamples); 
  }
  TRACE("Micro::Read(): END\n");
}

void* Micro::GetSoundData(int& length)
{
  length = sizeof(SAMPLE_TYPE) * SAMPLE_BUFFER_SIZE;
  return this->samples;
}

//void Micro::GetWave()
//{
// Envoyer l'en-tête WAV
//  for (uint8_t i = 0; i < sizeof(wavHeader); i++) {
//    Serial.write(pgm_read_byte_near(wavHeader + i));
//  }

  // Envoyer les données audio
//  for (uint16_t i = 0; i < sizeof(audioData); i++) {
//    Serial.write(pgm_read_byte_near(audioData + i));
//  }  
//}