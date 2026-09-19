# AI-Enabled Mine Subsidence Monitoring and Early Warning System

## SIH 2026 Project

An AI-enabled, low-cost monitoring system designed to continuously monitor ground conditions in underground coal mines, identify abnormal changes, and provide early warnings through a web-based dashboard.

---

## SIH Problem Statement

**Problem Statement ID:** 26025

**Title:**  
Development of an AI-enabled Low Cost Real Time Mine Subsidence Monitoring, Prediction and Early Warning System for Underground Coal Mines in India

**Theme:** Smart Automation

**Category:** Hardware

### Problem

Mine subsidence can develop due to changes in underground ground conditions and may create risks for mine workers, surface infrastructure and nearby areas.

Existing monitoring approaches such as satellite-based InSAR and UAV surveys can provide useful deformation information, but they may not provide continuous, low-cost, ground-level monitoring at multiple local locations.

There is a need for a practical system that can continuously collect ground-condition data, identify abnormal changes and support timely warning and decision-making.

---

# Proposed Solution

We propose a low-cost distributed monitoring system that uses sensors, ESP32, LoRa communication and AI-based analysis to monitor selected mine zones.

The system collects sensor data from monitoring nodes and sends the data wirelessly to a central gateway. The collected data can then be stored, analyzed and displayed on a web dashboard.

AI/ML techniques are planned to analyze sensor patterns, detect abnormal conditions and support risk prediction and early warning.

### Main Idea

**Sensors → ESP32 → LoRa → Gateway → Backend → AI Analysis → Risk Level → Dashboard → Alert**

The proposed system is intended to complement existing technologies such as satellite and UAV-based monitoring by providing continuous and localized ground-level monitoring.

---

# System Architecture


             MONITORING ZONE
                    │
                    ▼
        ┌──────────────────────┐
        │   Sensor Node        │
        │                      │
        │  MPU6050 / ADXL345   │
        │  Soil Moisture       │
        │                      │
        │       ESP32          │
        └──────────┬───────────┘
                   │
                   │ LoRa
                   ▼
        ┌──────────────────────┐
        │    LoRa Gateway      │
        └──────────┬───────────┘
                   │
                   ▼
        ┌──────────────────────┐
        │      Backend         │
        │    Python/FastAPI    │
        └──────────┬───────────┘
                   │
                   ▼
        ┌──────────────────────┐
        │      Database        │
        │     PostgreSQL       │
        └──────────┬───────────┘
                   │
                   ▼
        ┌──────────────────────┐
        │     AI / ML Layer    │
        │                      │
        │ Anomaly Detection    │
        │ Risk Prediction      │
        └──────────┬───────────┘
                   │
                   ▼
        ┌──────────────────────┐
        │    Web Dashboard     │
        │                      │
        │ Data + Risk + Alerts │
        └──────────────────────┘
