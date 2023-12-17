# memes
A utility specifically created to send memes from a meme folder to a Discord channel.

## Preparations

1. Create a `settings.h` file in the `src` folder
2. Put this into the file:
   ```
   #define DISCORD_TOKEN "<your token here>"
   #define DISCORD_CHANNEL "<channel ID here>"
   #define MEME_FOLDER_SRC "<path to your meme folder>"
   ```

3. Make sure to replace `<your token here>` with your Discord token. **DO NOT SHARE THIS TOKEN TO ANYONE, AS IT CAN ALLOW ATTACKERS TO LOG IN TO YOUR ACCOUNT.** To find your Discord token, follow these steps:
   3.1 Open up Discord in your browser
   3.2 Press Ctrl+Shift+I
   3.3 Go to the "Network" tab
   3.4 Send a message, anywhere
   3.5 Look for the "messages" request
   3.6 Click on it, and scroll down until you see "Request headers"
   3.7 Here, you can find the "Authorization" property. It should look like this: "Authorization: \<your token>"
4. Make sure to replace `<channel ID here>` with the destination channel ID. To get a channel ID, follow these steps:
   4.1 To get the channel ID, go to your user settings
   4.2 Click on the "Advanced" tab
   4.3 Enable "Developer mode"
   4.4 Right click a channel and click "Copy Channel ID"
5. Make sure to replace `<path to your meme folder>` too.