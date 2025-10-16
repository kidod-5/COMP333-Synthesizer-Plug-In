# Software Engineering Project

This document serves to collect the organizational and stylistic choice we have considered prior to the beginning of this project.  It will continue to change throughout the process and include changes in tech stack as we develop.

---

## 1. Technologies

## Tech Stack (Tentatively)

- **Backend:**
  - Python:
    - PyTorch and/or TensorFlow will be useful for prototyping ML algorithms and training
  - C++: (for low latency processing (<10ms))
    - Runs the actual audio engine
    - Uses JUCE to do buffer processing
- **Frontend:**
  - React (JavaScript)
  - JUCE for basic UI
  - OpenGL for visualizations
- **Database:** TBD

---

## 2. Project Structure

### Backend (Python)

The backend will be organized based on the tool usage. Controllers will each be placed in their own file, paths/routes will be in separate files, and supporting modules (e.g., utilities) will be grouped by functionality and named accordingly.

**Example:**
```
backend/
├── controllers/
│   ├── user_controller.py
│   └── auth_controller.py
├── routes/
│   ├── user_routes.py
│   └── auth_routes.py
└── utils/
    └── validators.py
```

### Frontend (React)

The frontend will work using a similar organization structure. Pages will be stored in a dedicated folder, utilities in their own folder, contexts in another, and page components grouped together.

**Example:**
```
frontend/src/
├── pages/
│   ├── HomePage.jsx
│   └── ProfilePage.jsx
├── contexts/
│   └── AuthContext.jsx
├── utils/
│   └── api.js
└── components/
    ├── NavBar.jsx
    └── Footer.jsx
```

---

## 3. Naming Conventions

### Variables

Variables should be lowercase nouns when defined with `let` or `const`. Constants should be written in ALL_CAPS, also as nouns.

**Examples:**
```javascript
let user = "Alice";
let profileData = {};
const MAX_USERS = 100;
const API_URL = "https://api.example.com";
```

### Functions

Functions must use camelCase, and names should be verbs that clearly describe their purpose.

**Examples:**
```javascript
function fetchUserData() {
  // fetch user data
}

function handleLogin(credentials) {
  // handle login
}
```

### Classes

Classes will likely not be heavily used in this project. If a class is created, the file should only contain that one class, and the file name should match the class name.

**Example:**
`UserManager.js`
```javascript
class UserManager {
  constructor() {
    this.users = [];
  }

  addUser(user) {
    this.users.push(user);
  }
}
```

---

## 4. Comments and Documentation

### File Headers

Each file must begin with a block comment that explains its purpose, what it contains, and any dependencies (and potentially library tools?).

**Example:**
```python
"""
user_controller.py
------------------
Handles user-related operations including
creation, authentication, and profile management.
Dependencies: auth_controller.py, database.py
"""
```

### Function Documentation

Each function must have a short 1–2 line comment describing what it does, along with its inputs and outputs. Every function signature should have clear and correct type hints. 

**Examples:**

Python:
```python
# Fetches user data from the database
# Input: user_id (int) -> Output: user object (dict)
def get_user(user_id: int) -> Dict[int, str]:
    ...
```

JavaScript:
```javascript
// Sends a login request to the backend
// Input: credentials (object) -> Output: <Response>
function loginUser(credentials) {
  return fetch(`${API_URL}/login`, {
    method: "POST",
    body: JSON.stringify(credentials),
  });
}
```

---

## 5. Cohesion and Coupling

We do not yet have a finalized plan for handling coupling and cohesion. Our goal is to maximize cohesion (group related logic together) while minimizing coupling avoid unnecessary cross-file interactions/dependencies. This section will be improve throughout the process especially.

---

## 6. Helpful Links

Python: https://peps.python.org/pep-0008/ 
React: https://airbnb.io/javascript/react/ 

---

## 7. Whats Left?

- Final choice of database (SQL vs NoSQL)  
- Error handling strategy  
- Testing framework and conventions  
- Deployment standards (lambda backend, vercel/netfily frontend?)

---

## 7. Branching

- Branches will be made for creating and debugging major aspects of the webpage
- There will be at least one active branch relevant to the primary focal points of development at a time
- Branches will be merged and closed upon completion of a code review and a conclusion made on the development of the tool. 
