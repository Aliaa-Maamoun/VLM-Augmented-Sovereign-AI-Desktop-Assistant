from flask import Flask, request, jsonify
import os

app = Flask(__name__)

# Directory where your audio will accumulate
AUDIO_FOLDER = "stored_audio"
os.makedirs(AUDIO_FOLDER, exist_ok=True)

# Static file path - 'ab' mode will continuously append data here
RAW_AUDIO_PATH = os.path.join(AUDIO_FOLDER, "live_stream.raw")

# Clear any previous test recordings when the server boots up fresh
if os.path.exists(RAW_AUDIO_PATH):
    os.remove(RAW_AUDIO_PATH)

@app.route('/audio', methods=['POST'])
def handle_audio():
    try:
        # Extract the raw binary payload sent by the ESP32
        audio_chunk = request.data
        
        if not audio_chunk:
            return jsonify({"status": "error", "message": "Empty payload"}), 400

        # Append ('ab') mode glues the 4000-byte packets into one continuous file
        with open(RAW_AUDIO_PATH, "ab") as f:
            f.write(audio_chunk)
            
        print(f"[Server Ingestion] Captured packet: {len(audio_chunk)} bytes appended. Total file size: {os.path.getsize(RAW_AUDIO_PATH)} bytes")
        return jsonify({"status": "success"}), 200

    except Exception as e:
        print(f"[Server Error] Breakdown: {str(e)}")
        return jsonify({"status": "error", "message": str(e)}), 500

if __name__ == '__main__':
    # Listen on all local interfaces (vital for hotspot clients)
    app.run(host='0.0.0.0', port=5000, debug=False)