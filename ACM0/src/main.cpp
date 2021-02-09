#include <Arduino.h>
#include <ArduinoJson.h>
#include <Controllino.h>
// For analog button detection
#include "AnalogMultiButton.h"
// For MQTT communication
#include <Ethernet.h>
#include <PubSubClient.h>
// For pushing digital buttons
#include <Button.h>
#include <ButtonEventCallback.h>
#include <PushButton.h>
#include <Bounce2.h> // https://github.com/thomasfredericks/Bounce-Arduino-Wiring
// settings for individual home
#include <SettingsRez.h>

EthernetClient ethClient;
PubSubClient client(ethClient);
long lastReconnectAttempt = 0;

// main toggle function for lights (and others)
void toggle(int pin)
{
    digitalWrite(pin, !digitalRead(pin));
}

// Find which output to choose for the topic received
// TODO: need to rewrite this to struct array of int and string
int returnPin(const char *recEntity)
{
    for (unsigned int i = 0; i < TOTAL_ENTITY_DEF_ARRAY; i++)
    {
        if (strcmp(recEntity, ENTITY_DEF_ARRAY[i]) == 0)
        {
            return OUTPUT_DEF_ARRAY[i];
        };
    };

    return 65535;
}

// MQTT callback
void callback(char *topic, byte *payload, unsigned int length)
{
    // TODO: new way of handling topic(s) from HASS (ACM0/light/kitchen/island)
    // ! it still does not work, although commented out turns the lights on, but gets stuck...
    // ! we are still missing the way to handle toggle by the switch, so this needs to be rethought... :(
    // Split the topic into digestable fractions (ACM0/light/kitchen/island)

    //Serial.println(sscanf(recTopic, "%s/%s/%s", arduino, entityType, entity));
    /*
    char *topicBuffer[10], *token;
    char *recTopic = NULL;
    strcpy(recTopic, (const char *)topic);
    char *saveptr1;
    int j;
    for (j = 0; ; j++, recTopic = NULL)
    {
        token = strtok_r(recTopic, "/", &saveptr1);
        if (token == NULL)
            break;
        topicBuffer[j] = token;
        Serial.println(topicBuffer[j]);
    }
    char *arduino = topicBuffer[0];
    char *entityType = topicBuffer[1];
    char *entity = topicBuffer[2];
    /*
    for (unsigned int x = 3; x < 10; x++)
    {
        Serial.println(topicBuffer[x]);
        if (topicBuffer[x] == NULL)
            break;
        strcat(entity, "/");
        strcat(entity, topicBuffer[x]);
    }

    Serial.print("Received message for topic: ");
    Serial.print(arduino);
    Serial.print(" - ");
    Serial.print(entityType);
    Serial.print(" - ");
    Serial.println(entity);

    // Construct entity and state topic for the received entity
    char *stateTopic = topic;
*/
    char *concatEntity = topic;
    //strcpy(concatEntity, entityType);
    //strcat(concatEntity, "/");
    //strcat(concatEntity, entity);
    /*    strcpy(stateTopic, (const char *)topic);
    strcat(stateTopic, "/state");

    Serial.print("state topic: ");
    Serial.println(stateTopic);
    Serial.println(topic);
*/
    // Construct entity for pin checkout
    //char *concatEntity = 0;
    //Serial.println(sprintf(concatEntity, "%s/%s", entityType, entity));

    Serial.print("concatenated topic: ");
    Serial.println(concatEntity);

    // Define incoming message

    char message_buff[100];

    for (unsigned int i = 0; i < length; i++)
    {
        message_buff[i] = payload[i];
    }
    message_buff[length] = '\0';

    char *message = (char *)(message_buff);

    //to be removed
    Serial.print("message: ");
    Serial.println(message);
    //end of to be removed

    // Check the type to know what to do (light/outlet/blind)
    //if (strcmp(entityType, "light") == 0 || strcmp(entityType, "outlet") == 0)
    //{
    // Get the pin by the entity
    int foundPin = returnPin(concatEntity);
    Serial.print("PIN Found: ");
    Serial.println(foundPin);
    if (strcmp(message, "on") == 0 && digitalRead(foundPin) == LOW)
    {
        toggle(foundPin);
        client.publish(topic, "on");
    }
    else if (strcmp(message, "off") == 0 && digitalRead(foundPin) == HIGH)
    {
        toggle(foundPin);
        client.publish(topic, "off");
    }
    else
    {
        client.publish(topic, digitalRead(foundPin) == HIGH ? "on" : "off");
    }
    //};

    // ! This part can be removed if the above prooves to be working...
    /*
    const size_t capacity = JSON_ARRAY_SIZE(10) + JSON_OBJECT_SIZE(2) + 30;
    DynamicJsonDocument root(capacity);
    deserializeJson(root, payload);

    const char *action = root["action"];
    if (strcmp(action, "toggle") == 0)
    {
        for (unsigned int i = 0; i < root["output"].size(); i++)
        {
            const int button = root["output"][i];
            if (button != 0)
            {
                toggle(button);
                Serial.print(button);
                Serial.print("--");
                Serial.println(digitalRead(button));
            }
        }
    }

    // state handling
    if (strcmp(action, "state") == 0)
    {
        // construct JSON object - state
        const size_t stateCapacity = JSON_ARRAY_SIZE(30) + JSON_OBJECT_SIZE(2) + 30;
        DynamicJsonDocument state(stateCapacity);

        // add controllino key
        state["controllino"] = "ACM0";
        //state["timestamp"] = millis();

        // construct supporting JSON object - state_data
        const size_t stateCapacity2 = JSON_ARRAY_SIZE(0) + JSON_OBJECT_SIZE(30) + 30;
        DynamicJsonDocument state_data(stateCapacity2);

        // output should be all the outputs whose state is requested e.g.:[2,3,78,80] or [all]
        for (unsigned int y = 0; y < root["output"].size(); y++)
        {
            // when the request for state is for all devices connected
            if (strcmp(root["output"][y], "all") == 0)
            {
                // output of all the digital outputs connected to controllino
                for (unsigned int st = 0; st < TOTAL_OUTPUT_DEF_ARRAY; st++)
                {
                    int output = OUTPUT_DEF_ARRAY[st];
                    bool outputState = digitalRead(output);

                    // write state to JSON document
                    state_data[String(output)].set(outputState);
                }
                // when the request for state is for few particular digital output pin(s)
            }
            else
            {
                int output = root["output"][y];
                bool outputState = digitalRead(output);

                // write state to JSON document
                state_data[String(output)].set(outputState);
            }
        }

        // convert Json document to String to be inserted into main Json
        String state_data2pub = "";
        serializeJson(state_data, state_data2pub);
        // insert state_data into main Json - state
        state["state_data"].set(serialized(state_data2pub));

        // to be removed when working (testing only)
        // print JSON onto Serial interface
        Serial.println("JSON Pretty:");
        String state2pub = "";
        serializeJsonPretty(state, state2pub);
        Serial.println();
        // end (to be removed)

        // publish state to requestor
        // TODO: convert state2pub to char*
        client.publish("STATE", state2pub);
        // ... resubscribe
        client.subscribe("ACM0");
    }
*/
    //if (strcmp(action, "info") == 0)
    if (strcmp(topic, "ACM0/info/#") == 0)
    {
        Serial.println("ACM0 reconnected...'info' command received.");
        client.publish("ACM0/info_done", "0");
    }
}

boolean reconnect()
{
    // Serial.println("Attempting deviceACM1 - MQTT connection ...");
    if (client.connect("deviceACM0"))
    {
        // Once connected, publish an announcement...
        //client.publish("ACM0", "{\"action\":\"info\"}");
        client.publish("ACM0/info/0", "info on");
        // ... and resubscribe
        client.subscribe("ACM0/#");
    }
    return client.connected();
}

// Use this function to configure the internal Bounce object to suit you. See the documentation at: https://github.com/thomasfredericks/Bounce2/wiki
// This function can be left out if the defaults are acceptable - just don't call configureButton
void configurePushButton(Bounce &bouncedButton)
{
    // Set the debounce interval to 15ms - default is 10ms
    bouncedButton.interval(15);
}

// btn is a reference to the button that fired the event. That means you can use the same event handler for many buttons
void checkPressedPushButton(Button &btn)
{
    for (int p = 0; p < PUSH_BUTTONS_TOTAL; p++)
    {
        if (PUSH_BUTTONS_DEF[p].is(btn))
        {
            client.publish(PUSH_BUTTONS_ACT[0][p], PUSH_BUTTONS_ACT[1][p]);
            client.subscribe("ACM0/#");
        }
    }
}

void setup()
{
    // begin serial so we can see which buttons are being pressed through the serial monitor
    Serial.begin(9600);
    Serial.println("Init");

    // create MQTT
    client.setServer(server, 1883);
    client.setCallback(callback);

    client.subscribe("ACM0/#");

    // Setup ethernet
    Ethernet.begin(mac, ip);
    delay(200);
    lastReconnectAttempt = 0;

    // Configure the button as you'd like - not necessary if you're happy with the defaults
    for (int p1 = 0; p1 < PUSH_BUTTONS_TOTAL; p1++)
    {
        PUSH_BUTTONS_DEF[p1].configureButton(configurePushButton);
    }

    // When the button is first pressed, call the function checkPressedPushButton (above)
    for (int p2 = 0; p2 < PUSH_BUTTONS_TOTAL; p2++)
    {
        PUSH_BUTTONS_DEF[p2].onPress(checkPressedPushButton);
    }
}

void loop()
{
    // Digital
    for (int p3 = 0; p3 < PUSH_BUTTONS_TOTAL; p3++)
    {
        PUSH_BUTTONS_DEF[p3].update();
    }

    // Analog
    buttonsA0.update();
    buttonsA1.update();
    buttonsA2.update();
    buttonsA4.update();
    buttonsA13.update();
    buttonsA14.update();
    buttonsA15.update();

    // Check if button is pressed
    for (int aBarray = 0; aBarray < ANALOG_BUTTONS_TOTAL; aBarray++)
    {
        // dereference analog multi button reference
        AnalogMultiButton *p0 = ANALOG_BUTTONS_DEF[aBarray];
        AnalogMultiButton deRefObject = *p0;
        // cycle through all possible buttons
        for (int aB = 0; aB < BUTTONS_TOTAL; aB++)
        {
            if (deRefObject.onPress(aB))
            {
                client.publish(ANALOG_BUTTONS_ACT[0][aBarray][aB], ANALOG_BUTTONS_ACT[1][aBarray][aB]);
                client.subscribe("ACM0/#");
            }
        }
    }

    // MQTT connect & reconnect
    if (!client.connected())
    {
        long now = millis();
        if (now - lastReconnectAttempt > 5000)
        {
            lastReconnectAttempt = now;
            // Attempt to reconnect
            if (reconnect())
            {
                lastReconnectAttempt = 0;
            }
        }
    }
    else
    {
        // Client connected
        client.loop();
    }
}

// duration reports back how long it has been since the button was originally pressed.
// repeatCount tells us how many times this function has been called by this button.
void onButtonHeld(Button &btn, uint16_t duration, uint16_t repeatCount)
{
    Serial.print("button has been held for ");
    Serial.print(duration);
    Serial.print(" ms; this event has been fired ");
    Serial.print(repeatCount);
    Serial.println(" times");
}

// duration reports back the total time that the button was held down
void onButtonReleased(Button &btn, uint16_t duration)
{
    Serial.print("button released after ");
    Serial.print(duration);
    Serial.println(" ms");
}

/***/
