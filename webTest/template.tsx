import React, {useEffect, useState} from 'react';
import mqtt from "mqtt";

const template: React.FC = () => {
    const [moisture, setMoisture] = useState<string | null>(null);

    useEffect(() => {
        const brokerUrl = "wss://broker.emqx.io:8084/mqtt";
        const options = {
          clientId: `mqtt_${Math.random().toString(16).slice(3)}`,
          keepAlive: 60,
          clean: true,
          reconnectPeriod: 1000,
          connectTimeout: 30 * 1000,
        };
    
        const client = mqtt.connect(brokerUrl, options);
    
        const onConnect = () => {
            console.log("Connected to MQTT Broker");
            client.subscribe('tes-prophancer-sub/1', (err) => {
                if (err) {
                    console.log(err);
                } else {
                    console.log('Subscribed to topic: tes-prophancer-sub/1');
                }
            });
        };

        const onMessage = async (topic: string, message: Buffer) => {
            console.log('onMessage', message.toString());
            try {
                const data = JSON.parse(message.toString());
                const newMoisture = data.Moisture !== null ? data.Moisture.toFixed(2) : 'null';

                setMoisture(newMoisture);

                try {
                    const response = await fetch('/save-data', {
                        method: 'POST',
                        headers: {
                            'Content-Type': 'application/json',
                        },
                        body: JSON.stringify({Moisture: parseFloat(newMoisture)}),
                    });

                    const responseData = await response.text();
                    console.log(responseData);
                } catch (fetchError) {
                    console.error('Fetch Error:', fetchError);
                }
            } catch (err) {
                console.error('Failed to parse JSON:', err);
            }
        };
        client.on('connect', onConnect);
        client.on('message', onMessage);
    }, []);

    return (
        <div>
            <h1>ESP32 Sensor Data</h1>
            <div id="data">
                <div>Moisture: <span>{moisture} %</span></div>
            </div>
        </div>
    );
};

export default template;