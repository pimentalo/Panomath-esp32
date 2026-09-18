/*
  Un compte à rebours.

  setTime() initialise la durée
  start() commence à décompter
  suspend() met en pause
*/
CompteARebours::CompteARebours() { _pauseDuration = 0; _status = STOPPED; }

void CompteARebours::setTime(int minutes, int seconds)
{
  if (this->_status == STOPPED) {
    this->_totalTime_s = (minutes * 60 + seconds);
  } else {
    LOG_ERROR("CompteARebours::setTime() can't be called where not stopped");
  }
}
void CompteARebours::start()
{
  if (this->_status == STOPPED) {
    this->_startTime = millis();
    _pauseDuration = 0; 
  }
  this->_status = RUNNING;
}
void CompteARebours::pause()
{
  if (this->_status == RUNNING)
    this->_pauseTime = millis();
  this->_status = PAUSED;
}
void CompteARebours::resume()
{
  if (this->_status == PAUSED)
  {
    this->_pauseDuration += millis() - this->_pauseTime;
  }
  this->_status = RUNNING;
}

void CompteARebours::getString(char *buffer, int length)
{
  // If currently paused, computes current pause time
  long m = millis();
  long currentPauseTime = (this->_status == PAUSED) ? m - this->_pauseTime : 0;
  long elapsed = (m - currentPauseTime - this->_pauseDuration - this->_startTime) / 1000;
  long compteur = this->_totalTime_s - elapsed;
  uint8_t secondes = compteur % 60;
  uint8_t minutes = compteur / 60;
  snprintf(buffer, length, "%02d:%02d", minutes, secondes);
}