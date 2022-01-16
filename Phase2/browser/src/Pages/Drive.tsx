import { OAuth2Client } from 'google-auth-library';
import { google } from 'googleapis';

const CLIENTID = "779330860326-6r95d4rnomvmuch1e4tvveevaeiai6gr.apps.googleusercontent.com";
const CLIENTKEY = "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";
const REDIRECT_URI = "https://developers.google.com/oauthplayground";

const REFRESHTOKEN = "1//04QXuErRGczqJCgYIARAAGAQSNwF-L9IrF26oAvHfv96-9UUXvE_utQrDMo_7thokk-wSoC1ndCKe14XxAwrYW0L-JaX9-ou8s0w";

const Oauth2Client = new google.auth.OAuth2(
    CLIENTID,
    CLIENTKEY,
    REDIRECT_URI
);

Oauth2Client.setCredentials({refresh_token: REFRESHTOKEN});

const drive = google.drive({
    version: 'v3',
    auth: Oauth2Client
});

export default async function upload(file: File){
    try {  
        const response = await drive.files.create({
            requestBody: {
                name: 'test.png',
                mimeType: 'image/jpg',
            },
            media: {
                mimeType: 'image/jpg',
                body: file,
            },
        });
        console.log(response.data);
    } catch (e) {
        console.log(e);
    }
}