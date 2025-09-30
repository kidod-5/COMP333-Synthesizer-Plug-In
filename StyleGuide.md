# Software Engineering Project

This document establishes conventions for technologies, file organization, naming, and documentation across our project. Following these standards will ensure consistency, readability, and maintainability of our codebase.

---

## 1. Technologies

## Tech Stack (Tentatively)

- **Backend:** Python
- **Frontend:** React (JavaScript)
- **Database:** TBD 

---

## 2. Project Structure

### Backend (Python)

The backend will be organized by usage. Controllers will each be placed in their own file, paths/routes will live in separate files, and supporting modules (e.g., utilities) will be grouped by functionality.

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

The frontend will follow a similar usage-based organization. Pages will be stored in a dedicated folder, utilities in their own folder, contexts in another, and reusable components grouped together.

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

Classes will not be heavily used in this project. If a class is created, the file should only contain that one class, and the file name should match the class name.

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

Each file must begin with a block comment that explains its purpose, what it contains, and any dependencies.

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

Each function must have a short 1–2 line comment describing what it does, along with its inputs and outputs.

**Examples:**

Python:
```python
# Fetches user data from the database
# Input: user_id (int) -> Output: user object (dict)
def get_user(user_id):
    ...
```

JavaScript:
```javascript
// Sends a login request to the backend
// Input: credentials (object) -> Output: Promise<Response>
function loginUser(credentials) {
  return fetch(`${API_URL}/login`, {
    method: "POST",
    body: JSON.stringify(credentials),
  });
}
```

---

## 5. Cohesion and Coupling

We do not yet have a finalized plan for handling coupling and cohesion. Our **goal** is to maximize cohesion (group related logic together) while minimizing coupling (avoid unnecessary cross-file dependencies). This section will be updated as the project develops.

---

## 6. Future Considerations

- Final choice of database (SQL vs NoSQL)  
- Error handling strategy  
- Testing framework and conventions  
- Deployment standards (lambda backend, vercel/netfily frontend?)

---
