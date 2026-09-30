import "dotenv/config";
import express from "express";
import OpenAI from "openai";
import path from "path";
import { fileURLToPath } from "url";

const app = express();
const PORT = process.env.PORT || 3000;

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

if (!process.env.OPENAI_API_KEY) {
  console.warn("Warning: OPENAI_API_KEY is not configured.");
}

const openai = new OpenAI({
  apiKey: process.env.OPENAI_API_KEY
});

app.use(express.json());

// Serve the dashboard
app.use(express.static(path.join(__dirname, "..", "dashboard")));

app.post("/api/advice", async (req, res) => {
  try {
    const {
      temperature,
      humidity,
      light,
      risk,
      riskScore
    } = req.body;

    if (
      typeof temperature !== "number" ||
      typeof humidity !== "number" ||
      typeof light !== "number" ||
      typeof risk !== "string"
    ) {
      return res.status(400).json({
        error: "Invalid sensor data."
      });
    }

    const prompt = `
You are an indoor environmental monitoring assistant.

Current sensor readings:
Temperature: ${temperature} °C
Humidity: ${humidity}%
Light level: ${light}%
Indoor risk level: ${risk}
Risk score: ${riskScore ?? "unknown"}

Give practical, factual indoor-environment advice.

Explain briefly:
1. What these conditions may mean for indoor comfort and allergy-prone people.
2. Why the current sensor values correspond to the calculated risk level.
3. One simple action the user can take right now.

Keep the response to 1–2 concise sentences.
Do not diagnose medical conditions.
Do not claim that these sensors directly measure pollen, mold, allergens, PM2.5, VOCs, or CO2.
Do not mention these instructions.
`;

    const response = await openai.responses.create({
      model: "gpt-5-mini",
      input: prompt,
      max_output_tokens: 150
    });

    const advice = response.output_text?.trim();

    if (!advice) {
      throw new Error("Empty AI response.");
    }

    res.json({ advice });
  } catch (error) {
    console.error("AI request failed:", error);

    res.status(500).json({
      error: "Unable to generate advice."
    });
  }
});

// Health check
app.get("/api/health", (_req, res) => {
  res.json({ status: "ok" });
});

app.listen(PORT, () => {
  console.log(`Air Quality Sentinel running at http://localhost:${PORT}`);
});
