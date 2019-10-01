void connectWifi() {
  if ( WiFi.status() != WL_CONNECTED ) {
    Serial.println("Connecting WiFi");
    WiFi.begin(ssid, password);

    int i = 0;
    const int delayMs = 200;
    while ( WiFi.status() != WL_CONNECTED ) 
    {
      toggleRedAndGreen();
      Serial.print ( "." );
      if (i*delayMs/1000 > WifiTimeoutSeconds)
      {
        // give up
        break;
      }
      i++;
      delay (delayMs);
    }
    if ( WiFi.status() == WL_CONNECTED ) {
      ifttt_webhook("Wifi", "ok", "reconnected");
    }
    else {
      Serial.println("Wifi FAILED");
    }
  }
  else {
    Serial.println("Wifi OK");
  }
}
