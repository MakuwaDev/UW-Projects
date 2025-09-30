# Advanced Modeling on Synthetic Data

**Dataset:**  
An artificially generated dataset with **2000 samples**, **400 input features** (standard-scaled real values), and **two output variables**:  
- `class` – a binary classification target (0/1).  
- `output` – a continuous regression target.  

Both outputs have non-trivial dependencies on subsets of the input features.  

**Tasks:**  
1. **Baseline Models (6 pts):**  
   - Build logistic regression (classification) and linear regression (regression) models using all features without preprocessing.  
   - Evaluate generalization via learned methods (e.g., validation, cross-validation).  

2. **Advanced Classification (12 pts):**  
   - Improve upon the baseline with advanced methods, parameter optimization, and feature selection.  
   - Compare performance against the baseline and report selected features.  

3. **Advanced Regression (12 pts):**  
   - Develop an improved regression model using advanced techniques, optimization, and feature selection.  
   - Compare performance to the baseline and discuss variable importance.  

**Evaluation Requirement:**  
Your solution must include a function that processes a provided `validation_data.csv` file and computes:  
- Classification accuracy (baseline & best model).  
- R² score (baseline & best model).  

**Submission:**  
Submit a Jupyter Notebook with code, commentary, and results via **Moodle**. The notebook must run in **Google Colab**.  
