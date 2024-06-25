const { GoogleAuth } = require('google-auth-library');
const fs = require('fs');

// Load the service account key file
const keyFile = 'plasma-creek-422117-116bf2c30a15.json';
const key = JSON.parse(fs.readFileSync(keyFile));

// Create a new GoogleAuth client
const auth = new GoogleAuth({
  credentials: key,
  scopes: ['https://www.googleapis.com/auth/cloud-platform'],
});

// Obtain an access token
auth.getClient()
  .then(client => client.getAccessToken())
  .then(response => {
    const accessToken = response.token;
    console.log('Access Token:', accessToken);
    // Use the access token in your application
    // For example, you can make authenticated requests to Google Cloud services
  })
  .catch(err => {
    console.error('Error obtaining access token:', err);
  });