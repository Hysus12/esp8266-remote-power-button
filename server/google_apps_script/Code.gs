/**
 * Google Apps Script endpoint for ESP8266 Remote Wake
 *
 * How it works:
 * - Put a command value (e.g. "0", "1", "2" ...) in a single cell (default A1)
 * - This script exposes a public HTTPS endpoint that returns JSON: {"cmd":"<value>"}
 * - ESP8266 polls the endpoint; when cmd changes, it triggers a relay pulse
 *
 * Setup:
 * 1) Create a Google Sheet.
 * 2) Apps Script -> paste this file as Code.gs.
 * 3) Update SPREADSHEET_ID below.
 * 4) Deploy -> "New deployment" -> type "Web app"
 *    - Execute as: Me
 *    - Who has access: Anyone
 */

const SPREADSHEET_ID = "PUT_YOUR_SPREADSHEET_ID_HERE";
const SHEET_NAME = "sheet1";   // must match your sheet tab name
const CMD_CELL = "A1";         // where you store the command

function doGet(e) {
  const ss = SpreadsheetApp.openById(SPREADSHEET_ID);
  const sh = ss.getSheetByName(SHEET_NAME);
  const cmd = String(sh.getRange(CMD_CELL).getValue());

  const payload = {
    cmd: cmd,
    ts: new Date().toISOString()
  };

  return ContentService
    .createTextOutput(JSON.stringify(payload))
    .setMimeType(ContentService.MimeType.JSON);
}
