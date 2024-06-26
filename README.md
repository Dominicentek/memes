# memes
A utility specifically created to send memes from a meme folder to a Discord channel.

## Preparations

1. Install `curl`
2. Create a `settings.h` file in the `src` folder
3. Put this into the file:
   ```c
   #define DISCORD_TOKEN "<your token here>"
   #define MEME_FOLDER_SRC "<path to your meme folder>"
   ```

4. Make sure to replace `<your token here>` with your Discord token. **DO NOT SHARE THIS TOKEN TO ANYONE, AS IT CAN ALLOW ATTACKERS TO LOG IN TO YOUR ACCOUNT.** To find your Discord token, follow these steps:

   4.1 Open up Discord in your browser

   4.2 Press Ctrl+Shift+I

   4.3 Go to the "Network" tab

   4.4 Send a message, anywhere

   4.5 Look for the "messages" request

   4.6 Click on it, and scroll down until you see "Request headers"

   4.7 Here, you can find the "Authorization" property. It should look like this: "Authorization: \<your token>"

5. Make sure to replace `<path to your meme folder>` too.
6. Add a list of channels

   An example of how that might look like:
   ```c
   CHANNEL("Channel Name", "<channel ID here>")
   CHANNEL("Channel 2", "<channel ID here>")
   // and so on
   ```
7. Make sure to replace `<channel ID here>` with the destination channel ID. To get a channel ID, follow these steps:

   7.1 Skip step 7.2 and 7.3 if you have developer mode already enabled

   7.2 Click on the "Advanced" tab

   7.3 Enable "Developer mode"

   7.4 Right click a channel and click "Copy Channel ID"
