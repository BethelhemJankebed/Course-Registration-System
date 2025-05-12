<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>Course Registration System</title>
</head>
<body>

  <h1>Course Registration System</h1>

  <h2>Overview</h2>
  <p>
    The <strong>Course Registration System</strong> is a C++ application using SQLite to manage course enrollment for students and course administration by admins. 
    It ensures students meet all prerequisite requirements before registering for courses, providing a reliable way to handle academic registration processes.
  </p>

  <h2>Features</h2>
  <ul>
    <li>Student login and course registration</li>
    <li>Prerequisite checking before enrollment</li>
    <li>Administrator login to manage courses and students</li>
    <li>Add, update, and delete courses</li>
    <li>Define and manage course prerequisites</li>
  </ul>

  <h2>Technologies Used</h2>
  <ul>
    <li><strong>C++</strong> for application logic</li>
    <li><strong>SQLite</strong> for database storage</li>
  </ul>


  <h2>Database</h2>
  <p>The system uses an SQLite database file (<code>courses.db</code>) which includes the following tables:</p>
  <ul>
    <li><code>students</code></li>
    <li><code>courses</code></li>
    <li><code>registrations</code></li>
  </ul>
  <p>The database is created and initialized automatically if it doesn't exist.</p>

  <h2>License</h2>
  <p>This project is for educational purposes.</p>

</body>
</html>
