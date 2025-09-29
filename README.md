## 💻 # arduino-cpp-google-sheet-api-integration

**ESP32 Sending Sensor Readings to Google Sheets**

### I. OBJECTIVE OF THE PRACTICUM
To send sensor reading results to a Google Sheet.

---

### II. TOOLS AND MATERIALS
1. Laptop/PC with internet connection
2. Smartphone (Android)

---

### III. THEORETICAL BASIS
In this project, you will learn how to **securely and reliably log data to Google Sheets** with an ESP32 using a **Google Service Account** and the **Google Sheets API**. We will use the **Arduino Google Sheets Client Library**. After explaining the most important basic concepts, we will create a Datalogger that saves temperature, humidity, pressure, and the associated timestamp to a Google spreadsheet.

---

### IV. PRACTICUM STEPS

#### A. Creating a Google Service Account
To securely log data to Google Sheets, we will use a **Google Service Account**. A Service Account, identified by its unique email address, is a special type of account. It's typically used by applications or computational workloads, such as Compute Engine instances, and is **not associated with a person**. You can learn more about service accounts.

1.  You need to **create a Google project** and associate the Google Service Account with that specific project. You can do this in your main Google account or choose to do it in another secondary account. As usual, we recommend you use a separate account only for IoT and ESP32 projects, and not your main account.
2.  Go to the **Google Cloud Console**, then **create a new project** or select an existing one. We will show you how to create a new project.
3.  **Name your project**. Then, click **Create**.
4.  Your project will be successfully created.
5.  Now, you need to **create a Service Account** for the project. In the left sidebar, click **Service Accounts**, then click **+ Create Service Account**.
6.  **Enter a name** for your Service Account, then click **Create and Continue**.
7.  **Select a role** for the Service Account. Choose **Owner**. Then, click **Continue**.
8.  Finally, click **Done**.
9.  You have successfully created a Service Account. Now, you need to **create a Key**.
10. Select the project, click the **three-dot icon**, then click **Manage Keys**.
11. Then, **Create New Key**.
12. Next, select **JSON** and click **Create**.
13. This will **download a JSON file** containing the key to your computer.
14. Open the file, and you will get something similar, but with your own details:
15. **Copy the `project_id`, `client_email`, `private_key_id`, and `private_key`** from the `.json` file because you will need them later.
16. Now that you have a project, you need to **enable the Google Sheets API** for that project. Click the following link: [https://console.cloud.google.com/apis/library/sheets.googleapis.com](https://console.cloud.google.com/apis/library/sheets.googleapis.com) and **enable the Google Sheets API** (you must use the same account where you created the project).
17. You also need to **enable the Google Drive API** for your project. Go to the following link: [https://console.cloud.google.com/apis/library/drive.googleapis.com](https://console.cloud.google.com/apis/library/drive.googleapis.com) and **enable the Google Drive API**.
18. Open **Google Sheets** and **create a new spreadsheet**. Give your spreadsheet a title. For example: **ESP32 Datalogging**.
19. **Save the spreadsheet ID**. The ID is highlighted in the image above. The Spreadsheet ID is the last string of characters in your spreadsheet's URL. For example, in the URL `https://docs.google.com/spreadsheets/d/1aISQE8K79LS5c3vF18qFRcmfDRFn_9nE4nKveWBCtoQ/edit#gid=0`, the spreadsheet ID is **`1aISQE8K79LS5c3vF18qFRcmfDRFn_9nE4nKveWBCtoQ`**.
20. For you to be able to log data to this spreadsheet using the Google Service Account, as we will do in this tutorial, you need to **share the spreadsheet with the Service Account email**. You will get the Service Account email in the JSON file you downloaded previously, stored in the **`client_email`** variable.
21. In the upper right corner, click **Share**. **Paste the Service Account email** and click **Send**.

---

#### B. ESP32 Coding and Wiring

1.  **Assemble the ESP32 and DHT11 sensor** as shown in the diagram.
    **Information:**
    a. DHT11 Data Pin connects to ESP32 pin **D4**
    b. DHT11 VCC Pin connects to **VCC**
    c. DHT11 GND Pin connects to **GND**
2.  **Download the `ESP-Google-Sheet-Client` library by Mobizt** in the Arduino IDE.
3.  **Write the following source code** and **make adjustments** for the following points:
    a. `WIFI_SSID`: the name of the Wi-Fi network you are using
    b. `WIFI_PASSWORD`: the password for the Wi-Fi network you are using
    c. `project_id`: the project ID obtained from the JSON file
    d. `client_email`: the client email obtained from the JSON file
    e. `private_key`: the private key obtained from the JSON file
    f. `id_spreadsheet`: your spreadsheet ID

**(Source Code Provided)**

---

### V. Assignment

1.  Show that the data transmission to the Google Sheet has been successful!
2.  Run the application, then observe by changing the temperature and humidity. How does the application work?

#### 1. RESULTS OF DATA INTEGRATION TO GOOGLE SHEET WITH OBSERVATIONS AND ADJUSTMENTS TO TEMPERATURE AND HUMIDITY CHANGES:

*(The response for this question is missing in the original document, but it would typically involve an image or screenshot of the Google Sheet with the successfully logged data.)*

#### 2. APPLICATION WORKFLOW:

This application works by using **Service Account Authentication** to automatically access the **Google Sheets API**. The process begins when the ESP32 connects to the Wi-Fi, then uses the service account credentials (`client_email` and `private_key`) to generate a **JWT (JSON Web Token)** which is sent to the Google OAuth2 server to obtain an **access token**.

The `ESP_Google_Sheet_Client` library automatically manages this authentication process by calling `GSheet.begin()`, which encrypts and signs the JWT using the RSA private key, and then exchanges it for a valid access token.

Once successfully authenticated (status `GSheet.ready()` is **true**), the ESP32 can **send the sensor data** (temperature, humidity, timestamp) in **JSON format** to the Google Sheets API endpoint using an **HTTP POST request** with an authorization header containing the access token. The Google API server verifies the token, checks the service account's permission for the spreadsheet, and if valid, adds the data as a new row in Sheet1, with the library automatically handling **token refresh** to keep the connection active and connected.
